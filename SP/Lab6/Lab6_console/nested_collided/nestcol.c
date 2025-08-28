/*++

Copyright (c) Microsoft Corporation

Module Name:

    nestcol.c

Abstract:

    This module implements a unit test for Nested and Collided SEH.

--*/

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

volatile BOOLEAN BreakOnEachStep = FALSE;
ULONG64 TestAccumulator;

#define START_TEST                                  \
{                                                   \
    if (BreakOnEachStep) {                          \
        __debugbreak();                             \
    }                                               \
                                                    \
    printf("START_TEST + 1 ");                      \
    TestAccumulator = 1;                            \
}

#define TEST_STEP(___PRIME)                         \
{                                                   \
    if (BreakOnEachStep) {                          \
        __debugbreak();                             \
    }                                               \
    printf("TEST_STEP: Prime = %d", (int)(___PRIME)); \
                                                    \
    TestAccumulator *= (___PRIME);                  \
}

#define END_TEST(___ANSWER)                         \
{                                                   \
    if (BreakOnEachStep) {                          \
        __debugbreak();                             \
    }                                               \
                                                    \
    if (TestAccumulator != (___ANSWER)) {           \
        __debugbreak();                             \
    }                                               \
}


void
just_raise (void)
{
    printf("just_raise");
    TEST_STEP(3)
    RaiseException(0x4000, 0, 0, NULL);
}

int
execute_handler_filter (EXCEPTION_POINTERS *pExcept)
{
    printf("execute_handler_filter");
    UNREFERENCED_PARAMETER(pExcept);

    TEST_STEP(5)
    return EXCEPTION_EXECUTE_HANDLER;
}

int
nested_exception_filter (EXCEPTION_POINTERS *pExcept)
{
    printf("nested_exception_filter");
    TEST_STEP(7)
    if (pExcept->ExceptionRecord->ExceptionCode == 0x4000) {
        RaiseException(0x4001, 0, 0, NULL);
    }

    return EXCEPTION_CONTINUE_SEARCH;
}

void
nested_exception (void)
{
    printf("nested_exception");
    __try
    {
        just_raise();
    }
    __except (nested_exception_filter(GetExceptionInformation()))
    {
        TEST_STEP(11)
    }
}

void
collided_unwind (void)
{
    printf("collided_unwind");
    __try
    {
        just_raise();
    }
    __finally
    {
        TEST_STEP(13)
        RaiseException(0x4001, 0, 0, NULL);
    }
}

int
__cdecl
main(void)
{

    printf("Collided Unwind test... ");
    START_TEST

    __try
    {
        collided_unwind();
    }
    __except (execute_handler_filter(GetExceptionInformation()))
    {
        TEST_STEP(17)
    }

    END_TEST(3 * 5 * 5 * 13 * 17)
    puts("PASSED.");

    printf("Nested Exception test... ");
    START_TEST

    __try
    {
        nested_exception();
    }
    __except (execute_handler_filter(GetExceptionInformation()))
    {
        TEST_STEP(19)
    }

    END_TEST(3 * 5 * 7 * 7 * 19)
    puts("PASSED.");

    return 0;
}