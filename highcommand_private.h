#ifndef HIGH_COMMAND_PRIVATE_H__
#define HIGH_COMMAND_PRIVATE_H__

// private stuffs

#define HC_NEW_META { 0, NULL, 0, 0, 0, NULL, NULL }

#define HC_MAX_OPTS_CAPACITY (64)
#define HC_INITIAL_OPTS_CAPACITY (8)
#define HC_RESIZE_AT (80) /* percent */

#define HC_META_NEARING_CAPACITY(meta) ((meta->next_index + 1) > (float) meta->capacity * (HC_RESIZE_AT / 100.0))
#define HC_ARG_ISNT_OPTION(arg) (arg != NULL && (strcmp(arg, "-") == 0 || strcmp(arg, "--") == 0 || arg[0] != '-'))

int hc_resize_opts_array_by_ref(hc_meta *meta);
char *hc_get_short_options_by_ref(hc_meta *meta);
struct option *hc_get_long_options_by_ref(hc_meta *meta);
hc_option *hc_get_option_by_ref(hc_meta *meta, char short_name);

// private utils

int hc_extract_argument(char *name);

#endif
