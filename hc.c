#include "hc.h"
#include "hc_private.h"

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <stdio.h>

// global members (public)

int hc_cleanup_at_exit = -1;

// global members (private)

static struct hc_meta internal_meta = HC_NEW_META;
static char *program_name = NULL;
static char *inside_command = NULL;

static int ran_once = 0;

// by ref

struct hc_meta hc_new_meta() {
    return (struct hc_meta) HC_NEW_META;
}

int hc_opt_by_ref(struct hc_meta *meta, char *short_name, char *long_name, char *help_text) {
    int err = hc_resize_opts_array_by_ref(meta);
    if (err != 0) return err;

    size_t short_length = strlen(short_name);
    size_t long_length = strlen(long_name);

    int has_arg = hc_extract_argument(short_name);
    if  (has_arg != hc_extract_argument(long_name)) {
        // TODO print informative error message
        return -1;
    }

    int short_has_leading_dash = (short_length > 0 && short_name[0] == '-') ? 1 : 0;
    int long_has_leadng_dash = (long_length > 1 && long_name[0] == '-' && long_name[1] == '-') ? 2 : 0;

    char *s = strndup(short_name + short_has_leading_dash, short_length - has_arg - short_has_leading_dash);
    char *l = strndup(long_name + long_has_leadng_dash, long_length - has_arg - long_has_leadng_dash);
    char *h = strdup(help_text);
    if (s == NULL || l == NULL || h == NULL) {
        free(s); free(l); free(h);
        return errno;
    }

    struct hc_option new_opt = { s, l, h, has_arg, 0, 0, NULL, 0 };
    meta->options[meta->next_index++] = new_opt;
    return 0;
}

int hc_run_by_ref(struct hc_meta *meta, int argc, char *argv[]) {
    if (argc > 1) {
        struct hc_command *command = hc_get_command_by_ref(meta, argv[1]);
        if (command != NULL) {
            meta->ran = 1;
            meta->argv0 = argv[0];
            meta->ran_command = argv[1];
            return hc_run_by_ref(&(command->meta), argc - 1, argv + 1);
        }
    }

    char *short_options = hc_get_short_options_by_ref(meta);
    struct option *long_options = hc_get_long_options_by_ref(meta);
    if (long_options == NULL || short_options == NULL) {
        free(short_options); free(long_options);
        return errno;
    }

    meta->argv0 = argv[0];

    int opt;
    struct hc_option *hc_opt;
    optind = 0;
    while ((opt = getopt_long(argc, argv, short_options, long_options, NULL)) != EOF) {
        hc_opt = hc_get_option_by_ref(meta, (opt != ':' ? opt : optopt));
        if (hc_opt != NULL) {
            if (hc_opt->has_argument == optional_argument && optarg == NULL) {
                if (optind < argc && HC_ARG_ISNT_OPTION(argv[optind]) && strcmp(argv[optind], "--") != 0) {
                    optarg = argv[optind];
                    optind++;
                }
            } else if (hc_opt->has_argument == required_argument && optarg != NULL) {
                if (strcmp(optarg, "--") == 0 && strcmp(argv[optind - 1], "--") == 0) {
                    opt = ':';
                    optarg = NULL;
                    optind--;
                }
            }
        }

        // opt == ':', missing argument
        // opt == '?', unknown option
        if (opt != '?' && hc_opt != NULL) {
            if (opt == ':' && hc_opt->has_argument == required_argument) {
                // TODO print informative warning message
                printf("Missing argument for '--%s'\n", hc_opt->long_name);
            } else {
                hc_opt->is_present = 1;
                hc_opt->level++;
                if (hc_opt->has_argument > 0 && optarg != NULL) {
                    free(hc_opt->value);
                    hc_opt->value = strdup(optarg);
                    if (hc_opt->value == NULL) {
                        free(long_options);
                        free(short_options);
                        return errno;
                    }
                    hc_opt->has_value = 1;
                }
            }
        } else {
            if (optopt == '\0') {
                // TODO print informative warning message
                printf("Unknown option: '%s'\n", argv[optind - 1]);
            } else {
                // TODO print informative warning message
                printf("Unknown option '-%c'\n", optopt);
            }
        }

        optopt = '\0';
    }

    meta->new_argc = argc - optind;
    meta->new_argv = argv + optind;
    meta->ran = 1;

    free(long_options);
    free(short_options);
    return 0;
}

struct hc_meta *hc_cmd_by_ref(struct hc_meta *meta, char *name) {
    struct hc_command *command = malloc(sizeof(struct hc_command));
    if (command == NULL) {
        return NULL;
    }
    command->name = strdup(name);
    if (command->name == NULL) {
        free(command);
        return NULL;
    }

    command->meta = hc_new_meta();

    command->next = meta->commands_head;
    meta->commands_head = command;

    return &(command->meta);
}

void hc_free_meta_by_ref(struct hc_meta *meta) {
    if (meta == NULL) return;
    meta->ran = 0;
    if (meta->capacity == 0) return;
    for (int i = 0; i < meta->next_index; i++) {
        free(meta->options[i].short_name);
        free(meta->options[i].long_name);
        free(meta->options[i].help_text);
        free(meta->options[i].value);
    }
    free(meta->options);
    meta->options = NULL;
    meta->next_index = 0;
    meta->capacity = 0;
    meta->new_argc = 0;
    meta->new_argv = NULL;
    meta->argv0 = NULL;
    struct hc_command *head = meta->commands_head;
    while (head != NULL) {
        free(head->name);
        hc_free_meta_by_ref(&(head->meta));
        struct hc_command *old = head;
        head = head->next;
        free(old);
    }
    return;
}

// global

void hc_opt(char *short_name, char *long_name, char *help_text) {
    if (!internal_meta.ran) {
        int result;
        if (inside_command) {
            struct hc_command *cmd = hc_get_command_by_ref(&internal_meta, inside_command);
            result = hc_opt_by_ref(&(cmd->meta), short_name, long_name, help_text);
        } else {
            result = hc_opt_by_ref(&internal_meta, short_name, long_name, help_text);
        }
        if (result != 0) {
            // TODO print informative error
        }
    }
}

void hc_cmd(char *name, hc_cmd_callback callback) {
    if (inside_command != NULL) {
        // TODO print informative error
        printf("Ignoring nested command '%s' (already inside command %s)\n", name, inside_command);
        return;
    }
    inside_command = strdup(name);
    if (hc_cmd_by_ref(&internal_meta, name) == NULL) {
        // TODO print informative error
        return;
    }
    callback();
    free(inside_command);
    inside_command = NULL;
}

struct hc_results hc_run(int argc, char *argv[]) {
    if (ran_once == 0) {
        if (hc_cleanup_at_exit == -1) {
            hc_cleanup_at_exit = 1;
        }
        if (hc_cleanup_at_exit) {
            if (atexit(hc_atexit)) {
                // TODO print informative error
            }
        }
        ran_once = 1;
    }
    program_name = argv[0];
    if (!internal_meta.ran) {
        int result = hc_run_by_ref(&internal_meta, argc, argv);
        if (result != 0) {
            // TODO print informative error
            return (struct hc_results) { .options = NULL };
        }
    } else {
        // TODO print informative warning
    }
    return hc_get_results();
}

struct hc_results hc_get_results() {
    if (internal_meta.ran) {
        if (internal_meta.ran_command) {
            struct hc_meta cmd_meta = hc_get_command_by_ref(&internal_meta, internal_meta.ran_command)->meta;
            return (struct hc_results) {
                .program_name = program_name,
                .name         = cmd_meta.argv0,
                .options      = cmd_meta.options,
                .count        = cmd_meta.next_index,
                .argc         = cmd_meta.new_argc,
                .argv         = cmd_meta.new_argv
            };
        }
        return (struct hc_results) {
            .program_name = program_name,
            .name         = internal_meta.argv0,
            .options      = internal_meta.options,
            .count        = internal_meta.next_index,
            .argc         = internal_meta.new_argc,
            .argv         = internal_meta.new_argv
        };
    } else {
        // TODO print informative warning
        return (struct hc_results) { .options = NULL };
    }
}

void hc_cleanup() {
    hc_free_meta_by_ref(&internal_meta);
    internal_meta = hc_new_meta();
}

int hc_is_present(char *long_name) {
    struct hc_option *hc_opt;
    if (internal_meta.ran_command) {
        struct hc_meta cmd_meta = hc_get_command_by_ref(&internal_meta, internal_meta.ran_command)->meta;
        hc_opt = hc_get_option_by_ref_long(&cmd_meta, long_name);
    } else {
        hc_opt = hc_get_option_by_ref_long(&internal_meta, long_name);
    }
    if (hc_opt != NULL) {
        return hc_opt->is_present;
    }
    return 0;
}

int hc_has_value(char *long_name) {
    struct hc_option *hc_opt;
    if (internal_meta.ran_command) {
        struct hc_meta cmd_meta = hc_get_command_by_ref(&internal_meta, internal_meta.ran_command)->meta;
        hc_opt = hc_get_option_by_ref_long(&cmd_meta, long_name);
    } else {
        hc_opt = hc_get_option_by_ref_long(&internal_meta, long_name);
    }    if (hc_opt != NULL) {
        return hc_opt->has_value;
    }
    return 0;
}

char *hc_get_value(char *long_name) {
    struct hc_option *hc_opt;
    if (internal_meta.ran_command) {
        struct hc_meta cmd_meta = hc_get_command_by_ref(&internal_meta, internal_meta.ran_command)->meta;
        hc_opt = hc_get_option_by_ref_long(&cmd_meta, long_name);
    } else {
        hc_opt = hc_get_option_by_ref_long(&internal_meta, long_name);
    }    if (hc_opt != NULL) {
        return hc_opt->value;
    }
    return NULL;
}

int hc_get_level(char *long_name) {
    struct hc_option *hc_opt;
    if (internal_meta.ran_command) {
        struct hc_meta cmd_meta = hc_get_command_by_ref(&internal_meta, internal_meta.ran_command)->meta;
        hc_opt = hc_get_option_by_ref_long(&cmd_meta, long_name);
    } else {
        hc_opt = hc_get_option_by_ref_long(&internal_meta, long_name);
    }    if (hc_opt != NULL) {
        return hc_opt->level;
    }
    return 0;
}

// private stuffs

int hc_resize_opts_array_by_ref(struct hc_meta *meta) {
    if (meta->options == NULL) {
        meta->options = malloc(HC_INITIAL_OPTS_CAPACITY * sizeof(struct hc_option));
        if (meta->options == NULL) {
            return errno;
        }
        meta->capacity = HC_INITIAL_OPTS_CAPACITY;
    } else if (HC_META_NEARING_CAPACITY(meta)) {
        if (meta->capacity * 2 > HC_MAX_OPTS_CAPACITY) {
            return -1; // too many options (shouldn't ever happen)
        }
        struct hc_option *buffer = realloc(meta->options, (meta->capacity * 2) * sizeof(struct hc_option));
        if (buffer != NULL) {
            meta->capacity *= 2;
            meta->options = buffer;
        } else {
            if (meta->next_index == meta->capacity) { // we're full
                return errno;
            } // else we still have room but we couldn't allocate memory this time
        }
    }
    return 0;
}

struct option *hc_get_long_options_by_ref(struct hc_meta *meta) {
    struct option terminator = { NULL, 0, NULL, 0 };
    struct option *long_options = malloc((meta->next_index + 1) * sizeof(struct option));
    if (long_options == NULL) return NULL;
    for (int i = 0; i < meta->next_index; i++) {
        struct option opt = {
            .name    = meta->options[i].long_name,
            .has_arg = meta->options[i].has_argument,
            .flag    = NULL,
            .val     = meta->options[i].short_name[0]
        };
        long_options[i] = opt;
    }
    long_options[meta->next_index] = terminator;
    return long_options;
}

char *hc_get_short_options_by_ref(struct hc_meta *meta) {
    char *short_options = malloc(((meta->next_index * 3) + 2) * sizeof(char)); // make sure we have enough room
    if (short_options == NULL) return NULL;
    int length = 0;
    short_options[length++] = ':';
    char *colons = "::";
    for (int i = 0; i < meta->next_index; i++) {
        int colon_length = (2 - meta->options[i].has_argument);
        if (colon_length > 2 || colon_length < 0) colon_length = 2;
        length += sprintf(short_options + length, "%c%s", meta->options[i].short_name[0], colons + colon_length);
    }
    char *buffer = realloc(short_options, (length + 1) * sizeof(char));
    if (buffer == NULL) {
        free(short_options);
        return NULL;
    }
    short_options = buffer;
    short_options[length] = '\0';
    return short_options;
}

struct hc_option *hc_get_option_by_ref(struct hc_meta *meta, char short_name) {
    for (int i = 0; i < meta->next_index; i++) {
        if (meta->options[i].short_name[0] == short_name) {
            return meta->options + i;
        }
    }
    return NULL;
}

struct hc_option *hc_get_option_by_ref_long(struct hc_meta *meta, char *long_name) {
    for (int i = 0; i < meta->next_index; i++) {
        if (strcmp(meta->options[i].long_name, long_name) == 0) {
            return meta->options + i;
        }
    }
    return NULL;
}

struct hc_command *hc_get_command_by_ref(struct hc_meta *meta, char *name) {
    struct hc_command *head = meta->commands_head;
    while (head != NULL && strcmp(head->name, name) != 0) {
        head = head->next;
    }
    return head;
}

// private utils

int hc_extract_argument(char *name) {
    size_t length = strlen(name);
    if (length > 1 && name[length - 1] == '=') {
        return required_argument; // 1
    } else if (length > 2 && name[length - 2] == '=' && name[length - 1] == '?') {
        return optional_argument; // 2
    } else {
        return no_argument; // 0
    }
}
