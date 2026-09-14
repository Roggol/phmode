#include "testing.h"

#include <stdlib.h>

typedef struct TestCase {
    const char *name;
    TestFn fn;
} TestCase;

static TestCase *s_tests = NULL;
static int s_testCount = 0;
static int s_testCapacity = 0;

int g_currentTestFailed = 0;

void TestRegistry_Add(const char *name, TestFn fn)
{
    if (s_testCount == s_testCapacity) {
        s_testCapacity = s_testCapacity ? s_testCapacity * 2 : 16;
        s_tests = realloc(s_tests, sizeof(TestCase) * (size_t)s_testCapacity);
    }

    s_tests[s_testCount].name = name;
    s_tests[s_testCount].fn = fn;
    s_testCount++;
}

int TestRegistry_RunAll(void)
{
    int passed = 0;
    int failed = 0;

    for (int i = 0; i < s_testCount; i++) {
        g_currentTestFailed = 0;
        printf("RUN  %s\n", s_tests[i].name);
        s_tests[i].fn();

        if (g_currentTestFailed) {
            printf("FAIL %s\n", s_tests[i].name);
            failed++;
        } else {
            printf("PASS %s\n", s_tests[i].name);
            passed++;
        }
    }

    printf("\n%d passed, %d failed, %d total\n", passed, failed, s_testCount);
    return failed == 0 ? 0 : 1;
}
