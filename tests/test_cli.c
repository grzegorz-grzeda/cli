#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "cli.h"

static void null_test(void **state)
{
}

int main(int argc, char **argv)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(null_test),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}