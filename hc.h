#ifndef HIGH_COMMAND_H__
#define HIGH_COMMAND_H__

extern int hc_cleanup_at_exit;

struct hc_meta;
struct hc_command;

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
    char *ran_command;
    struct hc_option *options;
    int next_index;
    int capacity;
    int new_argc;
    char **new_argv;
    char *argv0;
    struct hc_command *commands_head;
};

struct hc_command {
    char *name;
    struct hc_meta meta;
    struct hc_command *next;
};

struct hc_results {
    char *program_name;
    char *name;
    struct hc_option *options;
    int count;
    int argc;
    char **argv;
    struct hc_command command;
};

typedef void (*hc_cmd_callback)();

// by ref

struct hc_meta hc_new_meta();
int hc_opt_by_ref(struct hc_meta *meta, char *short_name, char *long_name, char *help_text);
int hc_run_by_ref(struct hc_meta *meta, int argc, char *argv[]);
void hc_free_meta_by_ref(struct hc_meta *meta);
struct hc_meta *hc_cmd_by_ref(struct hc_meta *meta, char *cmd_name);

// global

void hc_opt(char *short_name, char *long_name, char *help_text);
void hc_cmd(char *name, hc_cmd_callback callback);
struct hc_results hc_run(int argc, char *argv[]);
struct hc_results hc_get_results();
void hc_cleanup();

int hc_is_present(char *long_name);
int hc_has_value(char *long_name);
char *hc_get_value(char *long_name);
int hc_get_level(char *long_name);

#endif
