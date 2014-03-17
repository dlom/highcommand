#include "highcommand.h"

int hc_opt(char *short_name, char *long_name, char *help_text) {
    // create option
    //   string stuffs
    char *s = strdup(short_name);
    char *l = strdup(long_name);
    hc_option new_opt = {s, l, help_text, strdup(hc_meta.current_scope)};
    // add option to current scope
    if (hc_meta.options == NULL) { // nothing
        hc_meta.size = HC_INITIAL_OPTS_SIZE;
        hc_meta.options = malloc(hc_meta.size * sizeof(hc_option));
    } else if (index + 1 == size) { // full
        hc_meta.size = hc_meta.size * 2;
        realloc(hc_meta.options, hc_meta.size * sizeof(hc_option));
    }
    // return success
    return 0;
}

int hc_cmd(char *name, hc_command_callback callback) {
    // create subcommand
    // add subcommand to current scope
    // return success
    return 0;
}

int hc_run(int argc, char *argv[]/*, hashmap *opts*/) {
    // retrieve all subcommands in current scope
    // retrieve all opts for current scope
    // check if argv has a subcommand, assuming argv[0] is either the name of the program or the name of the parent command
    // if there's a command
    //   set scope
    //   call the callback with argv[0] being the name of the command
    //   unset scope
    // else if there's options, run getopts with these opts
    // else if there's a default command, run that
    // else if default help, run that
    // else do nothing
    // free the current scope
    hc_free(hc_meta.current_scope);
    // return success
    return 0;
}

int hc_free(const char *scope) {
    // get all opts/commands in a scope
    // reclaim that hot memory
    return 0;
}

#ifdef HIGH_COMMAND_TEST
#include <stdio.h>
#include "testhelp.h"

int main(int argc, char *argv[]) {
    hc_opt("n=", "name=", "What is your name?");
    hc_run(argc, argv);
    return 0;
}

#endif
