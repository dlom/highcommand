#include "highcommand.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    hc_meta meta = hc_new_meta();

    hc_opt_by_ref(&meta, "n=", "name=", "your name");
    hc_opt_by_ref(&meta, "o=?", "opt=?", "optional arguments");
    hc_opt_by_ref(&meta, "h", "help", "show this message");

    hc_run_by_ref(&meta, argc, argv);

    for (int i = 0; i < meta.next_index; i++) {
        hc_option hc_opt = meta.options[i];
        if (hc_opt.not_missing) {
            if (hc_opt.has_argument == no_argument) {
                printf("Option - %s (%d)\n", hc_opt.long_name, hc_opt.occurrences);
            } else {
                printf("Option - %s: '%s'\n", hc_opt.long_name, hc_opt.value);
            }
        }
    }

    hc_free_meta_by_ref(&meta);

    return 0;
}
