#include "highcommand.h"

int hc_opt_by_ref(hc_meta *meta, char *short_name, char *long_name, char *help_text) {
    // create option
    //   string stuffs
    char *s = strdup(short_name);
    char *l = strdup(long_name);
    char *h = strdup(help_text);
    //   crazy string shit happens here
    hc_option new_opt = {s, l, h};
    // check for size of opt array
    if (meta->options == NULL) { // create
        meta->capacity = HC_INITIAL_OPTS_CAPACITY;
        meta->options = malloc(meta->capacity * sizeof(hc_option));
        if (meta->options == NULL) {
            return errno;
        }
    } else if ((float) meta->capacity * (HC_RESIZE_FACTOR / 100.0) < (meta->current_index + 1)) { // resize
        meta->capacity *= 2;
        hc_option *buffer = realloc(meta->options, meta->capacity);
        if (buffer != NULL) {
            meta->options = buffer;
        } else {
            if (meta->capacity == meta->current_index + 1) {
                return errno;
            }
        }
    }
    meta->options[meta->current_index++] = new_opt;
    return 0;
}

int hc_run_by_ref(hc_meta *meta, int argc, char *argv[]/*, hashmap *opts*/) {
    // retrieve all opts
    // if there's options, run getopts with these opts
    // free everything
    hc_free_by_ref(meta);
    // return success
    return 0;
}

int hc_free_by_ref(hc_meta *meta) {
    int i;
    for (i = 0; i <= meta->current_index; i++) {
        free(meta->options[i].short_name);
        free(meta->options[i].long_name);
        free(meta->options[i].help_text);
    }
    free(meta->options);
    meta->options = NULL;
    return 0;
}

#ifdef HIGH_COMMAND_TEST
#include <stdio.h>
#include "testhelp.h"

int main(int argc, char *argv[]) {
    hc_meta meta = HC_META_NEW;
    test_cond("meta has null options", meta.options == NULL);
    test_cond("meta has 0 capacity", meta.capacity == 0);
    test_cond("meta has 0 index", meta.current_index == 0);

    hc_opt_by_ref(&meta, "n=", "name=", "the name bro");
    test_cond("meta has non-null options", meta.options != NULL);
    test_cond("meta has 8 capacity", meta.capacity == 8);
    test_cond("meta has 1 index", meta.current_index == 1);
    test_cond("first options is 'n='", strcmp(meta.options[0].short_name, "n=") == 0);
    test_cond("first option is 'name='", strcmp(meta.options[0].long_name, "name=") == 0);
    test_cond("first option has help text 'the name bro'", strcmp(meta.options[0].help_text, "the name bro") == 0);

    hc_run_by_ref(&meta, argc, argv);
    test_cond("meta has null options", meta.options == NULL);

    test_report();
    return 0;
}

#endif
