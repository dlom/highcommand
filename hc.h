#ifndef HIGH_COMMAND_H__
#define HIGH_COMMAND_H__

struct hc_option {
    char *short_name;
    char *long_name;
    char *help_text;
    int has_argument;
    int is_present;
    int has_value;
    char *value;
    int level;
};

struct hc_meta {
    int ran;
    struct hc_option *options;
    int next_index;
    int capacity;
    int new_argc;
    char **new_argv;
    char *argv0;
};

struct hc_results {
    char *name;
    struct hc_option *options;
    int count;
    int argc;
    char **argv;
};

// by ref

struct hc_meta hc_new_meta();
int hc_opt_by_ref(struct hc_meta *meta, char *short_name, char *long_name, char *help_text);
int hc_run_by_ref(struct hc_meta *meta, int argc, char *argv[]);
void hc_free_meta_by_ref(struct hc_meta *meta);

// global

void hc_opt(char *short_name, char *long_name, char *help_text);
struct hc_results hc_run(int argc, char *argv[]);
struct hc_results hc_get_results();
void hc_cleanup();

int hc_is_present(char *long_name);
int hc_has_value(char *long_name);
char *hc_get_value(char *long_name);
int hc_get_level(char *long_name);

#endif
