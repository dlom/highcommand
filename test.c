#include "highcommand.h"
#include <stdio.h>


int main(int argc, char *argv[]) {
    hc_opt("h",   "help",  "show this message");
    hc_opt("n=",  "name=", "your name");
    hc_opt("o=?", "opt=?", "optional arguments");

    hc_run(argc, argv);

    hc_results results = hc_get_results();

    for (int i = 0; i < results.count; i++) {
        hc_option hc_opt = results.options[i];
        if (hc_opt.has_value) {
            if (hc_opt.has_argument == 0) {
                printf("Option - %s (%d)\n", hc_opt.long_name, hc_opt.level);
            } else {
                printf("Option - %s: '%s'\n", hc_opt.long_name, hc_opt.value);
            }
        }
    }

    printf("Remaining arguments:\n");
    for (int i = 0; i < results.argc; i++) {
        printf("  %s\n", results.argv[i]);
    };

    hc_cleanup();

    return 0;
}
