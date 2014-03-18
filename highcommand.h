#ifndef __HIGH_COMMAND_H
#define __HIGH_COMMAND_H

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>

#define HC_MAX_OPTS_CAPACITY (1024)
#define HC_INITIAL_OPTS_CAPACITY (8)
#define HC_RESIZE_ON (80) /* percent */
#define HC_META_NEW {NULL, 0, 0}

#define HC_META_NEARING_CAPACITY(meta) ((meta->next_index + 1) > (float) meta->capacity * (HC_RESIZE_ON / 100.0))

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

// privatez

int _hc_free_by_ref(hc_meta *meta);
int _hc_resize_opts_array(hc_meta *meta);
int _hc_extract_argument(char *name);
struct option *_hc_create_getopt_long_array_by_ref(hc_meta *meta);

#endif
