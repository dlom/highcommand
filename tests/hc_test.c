#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <stdio.h>

#include "../hc.h"
#include "../hc_private.h"

static void test_hc(void **state) {
	(void) state; /* unused */
	assert_int_equal(0, 0);
}

int main() {
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(test_hc)
	};
	return cmocka_run_group_tests(tests, NULL, NULL);
}
