#include "highcommand.h"

int hc_opt_by_ref(hc_meta *meta, char *short_name, char *long_name, char *help_text) {
    int err = _hc_resize_opts_array(meta);
    if (err != 0) return err;
    char *s = strdup(short_name);
    char *l = strdup(long_name);
    char *h = strdup(help_text);
    hc_option new_opt = {s, l, h};
    meta->options[meta->next_index++] = new_opt;
    return 0;
}

int hc_run_by_ref(hc_meta *meta, int argc, char *argv[]) {
    // do nothing... yet
    _hc_free_by_ref(meta);
    return 0;
}

int _hc_free_by_ref(hc_meta *meta) {
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

int _hc_resize_opts_array(hc_meta *meta) {
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

#ifdef HIGH_COMMAND_TEST
#include <stdio.h>
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
    test_cond("first option is 'n='", strcmp(meta.options[0].short_name, "n=") == 0);
    test_cond("first option is 'name='", strcmp(meta.options[0].long_name, "name=") == 0);
    test_cond("first option has help text 'the name bro'", strcmp(meta.options[0].help_text, "the name bro") == 0);

    test_subject("enough options for meta resize");
    hc_opt_by_ref(&meta, "n=", "name=", "the name bro");
    hc_opt_by_ref(&meta, "n=", "name=", "the name bro");
    hc_opt_by_ref(&meta, "n=", "name=", "the name bro");
    hc_opt_by_ref(&meta, "d=", "different=", "wow different");
    hc_opt_by_ref(&meta, "n=", "name=", "the name bro");
    hc_opt_by_ref(&meta, "n=", "name=", "the name bro");
    test_cond("meta has 16 capacity", meta.capacity == 16);
    test_cond("meta has 7 as next index", meta.capacity == 16);
    test_cond("option 5 is still different", strcmp(meta.options[4].short_name, "d=") == 0);

    test_subject("run and free meta");
    hc_run_by_ref(&meta, argc, argv);
    test_cond("meta has null options", meta.options == NULL);
    test_cond("meta has 0 as next index", meta.next_index == 0);
    test_cond("meta has 0 capacity", meta.capacity == 0);

    test_report();
    return 0;
}

#endif
