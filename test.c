#include "hc.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    hc_opt("h",   "help",    "show this message");
    hc_opt("v",   "verbose", "verbosity (use multiple times)");
    hc_opt("n=",  "name=",   "your name");
    hc_opt("o=?", "opt=?",   "optional arguments");

    hc_results results = hc_run(argc, argv);

    if (hc_get_level("help")) {
        // display help
        printf("How to get this message: %s --help\n", results.name);
    }

    printf("Verbosity level set to %d\n", hc_get_level("verbose"));

    if (hc_has_value("name")) {
        printf("Hello %s\n", hc_get_value("name"));
    }

    if (hc_has_value("opt")) {
        printf("'%s' was the value passed to the optional flag\n", hc_get_value("opt"));
    }

    printf("Remaining arguments:\n");
    for (int i = 0; i < results.argc; i++) {
        printf("  %s\n", results.argv[i]);
    };

    hc_cleanup();

    return 0;
}
