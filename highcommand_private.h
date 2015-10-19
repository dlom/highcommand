#ifndef HIGH_COMMAND_PRIVATE_H__
#define HIGH_COMMAND_PRIVATE_H__

#define HC_MAX_OPTS_CAPACITY (64)
#define HC_INITIAL_OPTS_CAPACITY (8)
#define HC_RESIZE_ON (80) /* percent */


#define HC_META_NEARING_CAPACITY(meta) ((meta->next_index + 1) > (float) meta->capacity * (HC_RESIZE_ON / 100.0))
#define HC_ARG_ISNT_OPTION(arg) (arg != NULL && (strcmp(arg, "-") == 0 || strcmp(arg, "--") == 0 || arg[0] != '-'))

int hc_free_meta_by_ref(hc_meta *meta);
int hc_resize_opts_array_by_ref(hc_meta *meta);
char *hc_get_short_options_by_ref(hc_meta *meta);
struct option *hc_get_long_options_by_ref(hc_meta *meta);
hc_option hc_get_option_by_ref(hc_meta *meta, char short_name);

int hc_extract_argument(char *name);

#endif
