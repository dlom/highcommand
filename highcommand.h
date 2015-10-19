#ifndef HIGH_COMMAND_H__
#define HIGH_COMMAND_H__

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <stdio.h>

typedef struct {
    char *short_name;
    char *long_name;
    char *help_text;
    int has_argument;
    int not_missing;
    char *value;
    int occurrences;
} hc_option;

typedef struct {
    hc_option *options;
    int next_index;
    int capacity;
    int argc;
    char **argv;
} hc_meta;

// by ref

#define hc_new_meta() ((hc_meta){NULL, 0, 0})
int hc_opt_by_ref(hc_meta *meta, char *short_name, char *long_name, char *help_text);
int hc_run_by_ref(hc_meta *meta, int argc, char *argv[]);
int hc_free_meta_by_ref(hc_meta *meta);

#endif
