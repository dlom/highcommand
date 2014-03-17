#ifndef __HIGH_COMMAND_H
#define __HIGH_COMMAND_H

#include <stdlib.h>
#include <stddef.h>

#define HC_MAX_OPTS_SIZE (1024);
#define HC_INITIAL_OPTS_SIZE (8);

typedef struct {
    const char *short_name;
    const char *long_name;
    const char *help_string;
    const char *scope;
} hc_option;

typedef void (*hc_command_callback)(int argc, char *argv[]);

struct {
    const hc_option *options;
    int index;
    int size;
    const char *current_scope;
} hc_meta = {NULL, 0, 0, ""};

int hc_opt(char *short_name, char *long_name, char *help_text);
int hc_cmd(char *name, hc_command_callback callback);
int hc_run(int argc, char *argv[]/*, hashmap *opts*/);
int hc_free(char *scope);

#endif
