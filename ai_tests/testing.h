#ifndef AI_TESTS_TESTING_H
#define AI_TESTS_TESTING_H

#include <stdio.h>
#include <string.h>

/*
 * A tiny, dependency-free test framework. To add a new scenario, just write:
 *
 *   TEST(my_new_scenario)
 *   {
 *       BattleMon mon = { ... };
 *       ASSERT_TRUE(AI_IsImmuneToMove(&mon, TYPE_WATER));
 *   }
 *
 * anywhere in any file under scenarios/ (or add a new .c file there — see
 * build.sh). It self-registers automatically; nothing else needs updating.
 */

typedef void (*TestFn)(void);

void TestRegistry_Add(const char *name, TestFn fn);
int TestRegistry_RunAll(void);

extern int g_currentTestFailed;

#define TEST(name)                                                   \
    static void test_##name(void);                                   \
    static void register_##name(void) __attribute__((constructor));  \
    static void register_##name(void)                                \
    {                                                                 \
        TestRegistry_Add(#name, test_##name);                         \
    }                                                                 \
    static void test_##name(void)

#define ASSERT_TRUE(cond)                                                        \
    do {                                                                         \
        if (!(cond)) {                                                           \
            printf("    FAIL %s:%d: expected true: %s\n", __FILE__, __LINE__, #cond); \
            g_currentTestFailed = 1;                                             \
        }                                                                        \
    } while (0)

#define ASSERT_FALSE(cond) ASSERT_TRUE(!(cond))

#define ASSERT_EQ(actual, expected)                                                              \
    do {                                                                                          \
        long a_ = (long)(actual);                                                                \
        long e_ = (long)(expected);                                                               \
        if (a_ != e_) {                                                                           \
            printf("    FAIL %s:%d: %s == %ld, expected %s == %ld\n", __FILE__, __LINE__, #actual, a_, #expected, e_); \
            g_currentTestFailed = 1;                                                              \
        }                                                                                          \
    } while (0)

#endif /* AI_TESTS_TESTING_H */
