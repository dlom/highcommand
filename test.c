#include "highcommand.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    hc_meta meta = HC_META_NEW;

    hc_opt_by_ref(&meta, "n=", "name=", "your name");
    hc_opt_by_ref(&meta, "o=?", "opt=?", "optional arguments");
    hc_opt_by_ref(&meta, "h", "help", "show this message");

    hc_run_by_ref(&meta, argc, argv);

    hc_free_meta_by_ref(&meta);

    return 0;
}
