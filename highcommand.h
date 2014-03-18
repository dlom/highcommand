#ifndef __HIGH_COMMAND_H
#define __HIGH_COMMAND_H

#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>

#define HC_MAX_OPTS_CAPACITY (1024)
#define HC_INITIAL_OPTS_CAPACITY (8)
#define HC_RESIZE_ON (80) /* percent */
#define HC_META_NEW {NULL, 0, 0}

#define HC_META_NEARING_CAPACITY(meta) ((float) meta->capacity * (HC_RESIZE_ON / 100.0) < (meta->current_index + 1))

typedef struct {
    char *short_name;
    char *long_name;
    char *help_text;
} hc_option;

typedef struct {
    hc_option *options;
    int current_index;
    int capacity;
} hc_meta;

int hc_opt_by_ref(hc_meta *meta, char *short_name, char *long_name, char *help_text);
int hc_run_by_ref(hc_meta *meta, int argc, char *argv[]);
int hc_free_by_ref(hc_meta *meta);

// privatez

int _hc_resize_opts_array(hc_meta *meta);

#endif
