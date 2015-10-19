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
    int has_value;
    char *value;
    int level;
} hc_option;

typedef struct {
    int ran;
    hc_option *options;
    int next_index;
    int capacity;
    int new_argc;
    char **new_argv;
    char *argv0;
} hc_meta;

typedef struct {
    char *name;
    hc_option *options;
    int count;
    int argc;
    char **argv;
} hc_results;

// by ref

hc_meta hc_new_meta();
int hc_opt_by_ref(hc_meta *meta, char *short_name, char *long_name, char *help_text);
int hc_run_by_ref(hc_meta *meta, int argc, char *argv[]);
int hc_free_meta_by_ref(hc_meta *meta);

// global

void hc_opt(char *short_name, char *long_name, char *help_text);
hc_results hc_get_results();
hc_results hc_run(int argc, char *argv[]);
void hc_cleanup();

int hc_has_value(char *long_name);
char *hc_get_value(char *long_name);
int hc_get_level(char *long_name);

#endif
