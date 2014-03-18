#include "highcommand.h"

// by ref

int hc_opt_by_ref(hc_meta *meta, char *short_name, char *long_name, char *help_text) {
    int err = _hc_resize_opts_array_by_ref(meta);
    if (err != 0) return err;

    int short_length = strlen(short_name);
    int long_length = strlen(long_name);

    int has_arg = _hc_extract_argument(short_name);
    if  (has_arg != _hc_extract_argument(long_name)) return -1;

    char *s = strndup(short_name, short_length - has_arg);
    char *l = strndup(long_name, long_length - has_arg);
    char *h = strdup(help_text);

    hc_option new_opt = {s, l, h, has_arg};
    meta->options[meta->next_index++] = new_opt;
    return 0;
}

int hc_run_by_ref(hc_meta *meta, int argc, char *argv[]) {
    struct option *long_options = _hc_get_long_options_by_ref(meta);
    char *short_options = _hc_get_short_options_by_ref(meta);
    int opt = 0, old_opterr = opterr;
    opterr = 0;
    while ((opt = getopt_long(argc, argv, short_options, long_options, NULL)) != -1) {
        printf("%c: %s\n", opt, optarg);
    }
    opterr = old_opterr;
    free(long_options);
    free(short_options);
    _hc_free_by_ref(meta);
    return 0;
}

// private stuffs

int _hc_free_by_ref(hc_meta *meta) {
    if (meta->options == NULL) return 0;
    int i;
    for (i = 0; i < meta->next_index; i++) {
        free(meta->options[i].short_name);
        free(meta->options[i].long_name);
        free(meta->options[i].help_text);
    }
    free(meta->options);
    meta->options = NULL;
    meta->next_index = 0;
    meta->capacity = 0;
    return 0;
}

int _hc_resize_opts_array_by_ref(hc_meta *meta) {
    if (meta->options == NULL) {
        meta->capacity = HC_INITIAL_OPTS_CAPACITY;
        meta->options = malloc(meta->capacity * sizeof(hc_option));
        if (meta->options == NULL) {
            return errno;
        }
    } else if (HC_META_NEARING_CAPACITY(meta)) {
        meta->capacity *= 2;
        hc_option *buffer = realloc(meta->options, meta->capacity * sizeof(hc_option));
        if (buffer != NULL) {
            meta->options = buffer;
        } else {
            if (meta->capacity == meta->next_index + 1) {
                return errno;
            } // else we still have room but we couldn't allocate memory this time
        }
    }
    return 0;
}

struct option *_hc_get_long_options_by_ref(hc_meta *meta) {
    struct option terminator = {NULL, 0, NULL, 0};
    struct option *long_options = malloc((meta->next_index + 1) * sizeof(struct option));
    if (long_options == NULL) return NULL;
    int i;
    for (i = 0; i < meta->next_index; i++) {
        struct option opt = {meta->options[i].long_name, meta->options[i].has_argument, NULL, meta->options[i].short_name[0]};
        long_options[i] = opt;
    }
    long_options[i] = terminator;
    return long_options;
}

char *_hc_get_short_options_by_ref(hc_meta *meta) {
    char *short_options = malloc(((meta->next_index * 3) + 1) * sizeof(char)); // make sure we have enough room
    if (short_options == NULL) return NULL;
    int i, length = 0;
    for (i = 0; i < meta->next_index; i++) {
        char *colons = "::";
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

// private utils

int _hc_extract_argument(char *name) {
    size_t length = strlen(name);
    if (length > 1 && name[length - 1] == '=') {
        return 1; // required_argument
    } else if (length > 2 && name[length - 2] == '=' && name[length - 1] == '?') {
        return 2; // optional_argument
    } else {
        return 0; // no_argument
    }
}

#ifdef HIGH_COMMAND_TEST
#include "testhelp.h"

int main(int argc, char *argv[]) {
    test_subject("brand new meta");
    hc_meta meta = HC_META_NEW;
    test_cond("meta has null options", meta.options == NULL);
    test_cond("meta has 0 as next index", meta.next_index == 0);
    test_cond("meta has 0 capacity", meta.capacity == 0);

    test_subject("one option in meta")
    hc_opt_by_ref(&meta, "n=", "name=", "the name bro");
    test_cond("meta has non-null options", meta.options != NULL);
    test_cond("meta has 8 capacity", meta.capacity == 8);
    test_cond("meta has 1 as next index", meta.next_index == 1);
    test_cond("first option is 'n'", strcmp(meta.options[0].short_name, "n") == 0);
    test_cond("first option is 'name'", strcmp(meta.options[0].long_name, "name") == 0);
    test_cond("first option has help text 'the name bro'", strcmp(meta.options[0].help_text, "the name bro") == 0);
    test_cond("first option has non-optional argument", meta.options[0].has_argument == 1);

    test_subject("enough options for meta resize");
    hc_opt_by_ref(&meta, "a=", "amazing=", "it's amazing");
    hc_opt_by_ref(&meta, "b=?", "boom=?", "boom you're dead");
    hc_opt_by_ref(&meta, "c=", "crazy=", "that's crazy");
    hc_opt_by_ref(&meta, "d", "different", "wow different");
    hc_opt_by_ref(&meta, "e=", "epic=", "that was epic dude");
    hc_opt_by_ref(&meta, "f=", "fail=", "failure to succeed");
    test_cond("meta has 16 capacity", meta.capacity == 16);
    test_cond("meta has 7 as next index", meta.capacity == 16);
    test_cond("option 3 is still different", strcmp(meta.options[2].short_name, "b") == 0);
    test_cond("option 3 was altered", strcmp(meta.options[2].long_name, "boom") == 0);
    test_cond("option 3 has optional argument", meta.options[2].has_argument == 2);
    test_cond("option 5 is still different", strcmp(meta.options[4].short_name, "d") == 0);
    test_cond("option 5 wasn't altered", strcmp(meta.options[4].long_name, "different") == 0);
    test_cond("option 5 has no argument", meta.options[4].has_argument == 0);

    test_subject("generating data for getopt_long");
    char *short_options = _hc_get_short_options_by_ref(&meta);
    struct option *long_options = _hc_get_long_options_by_ref(&meta);
    test_cond("short optstring is valid", strcmp(short_options, "n:a:b::c:de:f:") == 0);
    test_cond("first option has valid name", strcmp(long_options[0].name, "name") == 0);
    test_cond("first option has required argument", long_options[0].has_arg == required_argument);
    test_cond("first option has no flag", long_options[0].flag == NULL);
    test_cond("first option has valid val", long_options[0].val == 'n');
    test_cond("third option has valid name", strcmp(long_options[2].name, "boom") == 0);
    test_cond("third option has optional argument", long_options[2].has_arg == optional_argument);
    test_cond("third option has no flag", long_options[2].flag == NULL);
    test_cond("third option has valid val", long_options[2].val == 'b');
    test_cond("fifth option has valid name", strcmp(long_options[4].name, "different") == 0);
    test_cond("fifth option has no argument", long_options[4].has_arg == no_argument);
    test_cond("fifth option has no flag", long_options[4].flag == NULL);
    test_cond("fifth option has valid val", long_options[4].val == 'd');
    free(short_options);
    free(long_options);

    test_subject("run and free meta");
    hc_run_by_ref(&meta, argc, argv);
    test_cond("meta has null options", meta.options == NULL);
    test_cond("meta has 0 as next index", meta.next_index == 0);
    test_cond("meta has 0 capacity", meta.capacity == 0);

    test_report();
    return 0;
}

#endif
