#include "highcommand.h"

int hc_opt(char *short_name, char *long_name, char *help_text) {
    // create option
    // add option to current scope
    // return success
}

int hc_cmd(char *name, hc_command_callback callback) {
    // add subcommand to current scope
    // set scope
    // call the callback with argv[0] being the name of the command
    // unset scope
    // return success
}

int hc_run(int argc, char *argv[]/*, hashmap *opts*/) {
    // retrieve all subcommands in current scope
    // retrieve all opts for current scope
    // check if argv has a subcommand, assuming argv[0] is either the name of the program or the name of the parent command
    // if there's a command, run it
    // else if there's options, run getopts with these opts
    // else if there's a default command, run that
    // else if default help, run that
    // else do nothing
    // return success
}

#ifdef HIGH_COMMAND_TEST
#include <stdio.h>
#include "testhelp.h"

int main(int argc, char *argv[]) {
    return 0;
}

#endif
