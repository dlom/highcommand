#ifndef HIGH_COMMAND_H__
#define HIGH_COMMAND_H__

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <stdio.h>

#define HC_META_NEW {NULL, 0, 0}

typedef struct {
    char *short_name;
    char *long_name;
    char *help_text;
    int has_argument;
} hc_option;

typedef struct {
    hc_option *options;
    int next_index;
    int capacity;
} hc_meta;

// by ref

int hc_opt_by_ref(hc_meta *meta, char *short_name, char *long_name, char *help_text);
int hc_run_by_ref(hc_meta *meta, int argc, char *argv[]);

#endif
