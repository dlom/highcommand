#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <stdio.h>
#include <stdlib.h>

#include "../hc.h"
#include "../hc_private.h"

static int setup(void **state) {
    struct hc_meta *test_meta = malloc(sizeof(struct hc_meta));
    if (test_meta == NULL) {
        return -1;
    }
    *test_meta = hc_new_meta();
    *state = test_meta;
    return 0;
}

static int teardown(void **state) {
    struct hc_meta *test_meta = *state;
    hc_free_meta_by_ref(test_meta);
    free(test_meta);
    return 0;
}

static void test_hc_new_meta(void **state) {
    struct hc_meta *test_meta = *state;
    struct hc_meta  test_empty_meta = HC_NEW_META;

    assert_memory_equal(test_meta, &test_empty_meta, sizeof(struct hc_meta));
}

static void test_hc_opt_by_ref(void **state) {
    struct hc_meta *test_meta = *state;

    assert_int_equal(test_meta->next_index, 0);
    assert_null(test_meta->options);

    hc_opt_by_ref(test_meta, "n=", "name=", "the name bro");
    assert_non_null(test_meta->options);
    assert_int_equal(test_meta->capacity, 8);
    assert_int_equal(test_meta->next_index, 1);
    assert_string_equal(test_meta->options[0].short_name, "n");
    assert_string_equal(test_meta->options[0].long_name,  "name");
    assert_string_equal(test_meta->options[0].help_text,  "the name bro");
    assert_int_equal(test_meta->options[0].has_argument, 1);

    hc_opt_by_ref(test_meta, "a=", "amazing=", "it's amazing");
    hc_opt_by_ref(test_meta, "b=?", "boom=?", "boom you're dead");
    hc_opt_by_ref(test_meta, "c=?", "crazy=?", "that's crazy");
    hc_opt_by_ref(test_meta, "d", "different", "wow different");
    hc_opt_by_ref(test_meta, "e=", "epic=", "that was epic dude");
    hc_opt_by_ref(test_meta, "f=", "fail=", "failure to succeed");
    hc_opt_by_ref(test_meta, "h", "help", "show this message");
    hc_opt_by_ref(test_meta, "v", "verbose", "verbosity level");
    assert_int_equal(test_meta->capacity,   16);
    assert_int_equal(test_meta->next_index, 9);
    assert_string_equal(test_meta->options[2].short_name, "b");
    assert_string_equal(test_meta->options[2].long_name, "boom");
    assert_int_equal(test_meta->options[2].has_argument, 2);
    assert_string_equal(test_meta->options[4].short_name, "d");
    assert_string_equal(test_meta->options[4].long_name, "different");
    assert_int_equal(test_meta->options[4].has_argument, 0);
}

static void test_hc_run_by_ref(void **state) {
    struct hc_meta *test_meta = *state;
    char *argv[] = {"name", "-a", "wow", "arg0", "--help", "-d", "--boom", "--epic", "wow2", "arg1", "arg2", "-vvvv", "--crazy"};
    int argc = sizeof(argv)/sizeof(argv[0]);
    hc_run_by_ref(test_meta, argc, argv);

    assert_string_equal(test_meta->argv0, "name");
    assert_int_equal(test_meta->new_argc, 3);
    assert_string_equal(test_meta->new_argv[0], "arg0");
    assert_string_equal(test_meta->new_argv[1], "arg1");
    assert_string_equal(test_meta->new_argv[2], "arg2");
    assert_true(test_meta->ran);
}

static void test_results(void **state) {
    struct hc_meta *test_meta = *state;

    assert_false(test_meta->options[0].is_present);
    assert_true(test_meta->options[1].is_present);
    assert_true(test_meta->options[2].is_present);
    assert_true(test_meta->options[3].is_present);
    assert_true(test_meta->options[4].is_present);
    assert_true(test_meta->options[5].is_present);
    assert_false(test_meta->options[6].is_present);
    assert_true(test_meta->options[7].is_present);
    assert_true(test_meta->options[8].is_present);

    assert_true(test_meta->options[1].has_value);
    assert_false(test_meta->options[2].has_value);
    assert_false(test_meta->options[3].has_value);
    assert_false(test_meta->options[4].has_value);
    assert_true(test_meta->options[5].has_value);
    assert_false(test_meta->options[7].has_value);
    assert_false(test_meta->options[8].has_value);

    assert_string_equal(test_meta->options[1].value, "wow");
    assert_string_equal(test_meta->options[5].value, "wow2");
    assert_int_equal(test_meta->options[8].level, 4);
}

static void test_edgecases(void **state) {
    struct hc_meta *test_meta = *state;
    hc_free_meta_by_ref(test_meta);
    *test_meta = hc_new_meta();

    hc_opt_by_ref(test_meta, "n=", "name=", "your name");
    hc_opt_by_ref(test_meta, "a=", "another=", "another one");
    hc_opt_by_ref(test_meta, "o=?", "optional=?", "optional argument");
    hc_opt_by_ref(test_meta, "m=?", "more=?", "more optional arguments");

    char *argv[] = {"name", "--name=--", "-a--", "--optional=--", "-m", "--", "arg0"};
    int argc = sizeof(argv)/sizeof(argv[0]);
    hc_run_by_ref(test_meta, argc, argv);

    assert_string_equal(test_meta->options[0].value, "--");
    assert_string_equal(test_meta->options[1].value, "--");
    assert_string_equal(test_meta->options[2].value, "--");
    assert_true(test_meta->options[3].is_present);
    assert_false(test_meta->options[3].has_value);
    assert_int_equal(test_meta->new_argc, 1);
    assert_string_equal(test_meta->new_argv[0], "arg0");
}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_hc_new_meta),
        cmocka_unit_test(test_hc_opt_by_ref),
        cmocka_unit_test(test_hc_run_by_ref),
        cmocka_unit_test(test_results),
        cmocka_unit_test(test_edgecases)
    };
    return cmocka_run_group_tests(tests, setup, teardown);
}
