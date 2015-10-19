#include "highcommand.h"
#include "highcommand_private.h"

// by ref

int hc_opt_by_ref(hc_meta *meta, char *short_name, char *long_name, char *help_text) {
    int err = hc_resize_opts_array_by_ref(meta);
    if (err != 0) return err;

    size_t short_length = strlen(short_name);
    size_t long_length = strlen(long_name);

    int has_arg = hc_extract_argument(short_name);
    if  (has_arg != hc_extract_argument(long_name)) return -1;

    char *s = strndup(short_name, short_length - has_arg);
    char *l = strndup(long_name, long_length - has_arg);
    char *h = strdup(help_text);
    if (s == NULL || l == NULL || h == NULL) {
        free(s); free(l); free(h);
        return errno;
    }

    hc_option new_opt = {s, l, h, has_arg, 0, NULL, 0};
    meta->options[meta->next_index++] = new_opt;
    return 0;
}

int hc_run_by_ref(hc_meta *meta, int argc, char *argv[]) {
    char *short_options = hc_get_short_options_by_ref(meta);
    struct option *long_options = hc_get_long_options_by_ref(meta);
    if (long_options == NULL || short_options == NULL) {
        free(short_options); free(long_options);
        return errno;
    }

    int opt;
    hc_option *hc_opt;
    while ((opt = getopt_long(argc, argv, short_options, long_options, NULL)) != EOF) {
        hc_opt = hc_get_option_by_ref(meta, (opt != ':' ? opt : optopt));
        if (hc_opt != NULL) {
            if (hc_opt->has_argument == optional_argument && optarg == NULL && HC_ARG_ISNT_OPTION(argv[optind])) {
                if (strcmp(argv[optind], "--") != 0) {
                    optarg = argv[optind];
                    optind++;
                }
            } else if (hc_opt->has_argument == required_argument && optarg != NULL && strcmp(optarg, "--") == 0) {
                opt = ':';
                optarg = NULL;
                optind--;
            }
        }

        // opt == ':', missing argument
        // opt == '?', unknown option
        if (opt != '?') {
            hc_opt->not_missing = 1;
            hc_opt->occurrences++;
            if (hc_opt->has_argument > 0 && optarg != NULL) {
                free(hc_opt->value);
                hc_opt->value = strdup(optarg);
            }
            if (opt == ':') printf("Missing argument for '--%s'\n", hc_opt->long_name);
        } else {
            if (optopt == '\0') {
                printf("Unknown option: '%s'\n", argv[optind - 1]);
            } else {
                printf("Unknown option '-%c'\n", optopt);
            }
        }

        optopt = '\0';
    }

    meta->argc = argc - optind;
    meta->argv = argv + optind;

    free(long_options);
    free(short_options);
    return 0;
}

int hc_free_meta_by_ref(hc_meta *meta) {
    if (meta->capacity == 0) return 0;
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
    return 0;
}

// private stuffs

int hc_resize_opts_array_by_ref(hc_meta *meta) {
    if (meta->options == NULL) {
        meta->capacity = HC_INITIAL_OPTS_CAPACITY;
        meta->options = malloc(meta->capacity * sizeof(hc_option));
        if (meta->options == NULL) {
            return errno;
        }
    } else if (HC_META_NEARING_CAPACITY(meta)) {
        if (meta->capacity * 2 > HC_MAX_OPTS_CAPACITY) {
            return -1;
        }
        hc_option *buffer = realloc(meta->options, (meta->capacity * 2) * sizeof(hc_option));
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

struct option *hc_get_long_options_by_ref(hc_meta *meta) {
    struct option terminator = {NULL, 0, NULL, 0};
    struct option *long_options = malloc((meta->next_index + 1) * sizeof(struct option));
    if (long_options == NULL) return NULL;
    for (int i = 0; i < meta->next_index; i++) {
        struct option opt = {meta->options[i].long_name, meta->options[i].has_argument, NULL, meta->options[i].short_name[0]};
        long_options[i] = opt;
    }
    long_options[meta->next_index] = terminator;
    return long_options;
}

char *hc_get_short_options_by_ref(hc_meta *meta) {
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

hc_option *hc_get_option_by_ref(hc_meta *meta, char short_name) {
    for (int i = 0; i < meta->next_index; i++) {
        if (meta->options[i].short_name[0] == short_name) {
            return meta->options + i;
        }
    }
    return NULL;
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
