#ifndef __HIGH_COMMAND_H
#define __HIGH_COMMAND_H

typedef struct {
    const char short_name; // not a typo
    const char *long_name;
    const char *help_string;
    const char *scope;
} hc_option;

typedef void (*hc_command_callback)(int argc, char *argv[]);

int hc_opt(char *short_name, char *long_name, char *help_text);
int hc_cmd(char *name, hc_command_callback callback);
int hc_run(int argc, char *argv[]/*, hashmap *opts*/);

#endif
