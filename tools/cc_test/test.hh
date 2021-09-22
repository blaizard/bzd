#pragma once

#include "cc_test/test_internal.hh"

#define TEST(testCaseName, testName) BZDTEST_(testCaseName, testName)

#define EXPECT_TRUE(condition) BZDTEST_TEST_BOOLEAN_(condition, condition, true, BZDTEST_FAIL_NONFATAL_)

#define EXPECT_FALSE(condition) BZDTEST_TEST_BOOLEAN_(!(condition), condition, false, BZDTEST_FAIL_NONFATAL_)

#define EXPECT_EQ(expression1, expression2) BZDTEST_TEST_EQ_(expression1, expression2, BZDTEST_FAIL_NONFATAL_)

#define EXPECT_NE(expression1, expression2) BZDTEST_TEST_NE_(expression1, expression2, BZDTEST_FAIL_NONFATAL_)

#define EXPECT_NEAR(number1, number2, absError) BZDTEST_TEST_NEAR_(number1, number2, absError, BZDTEST_FAIL_NONFATAL_)

#define EXPECT_STREQ(str1, str2) BZDTEST_TEST_STREQ_(str1, str2, BZDTEST_FAIL_NONFATAL_)

#define EXPECT_ANY_THROW(expression) BZDTEST_TEST_ANY_THROW_(expression, BZDTEST_FAIL_NONFATAL_)

#define ASSERT_TRUE(condition) BZDTEST_TEST_BOOLEAN_(condition, condition, true, BZDTEST_FAIL_FATAL_)

#define ASSERT_FALSE(condition) BZDTEST_TEST_BOOLEAN_(!(condition), condition, false, BZDTEST_FAIL_FATAL_)

#define ASSERT_EQ(expression1, expression2) BZDTEST_TEST_EQ_(expression1, expression2, BZDTEST_FAIL_FATAL_)

#define ASSERT_NE(expression1, expression2) BZDTEST_TEST_NE_(expression1, expression2, BZDTEST_FAIL_FATAL_)

#define ASSERT_NEAR(number1, number2, absError) BZDTEST_TEST_NEAR_(number1, number2, absError, BZDTEST_FAIL_FATAL_)

#define ASSERT_STREQ(str1, str2) BZDTEST_TEST_STREQ_(str1, str2, BZDTEST_FAIL_FATAL_)

#define ASSERT_ANY_THROW(expression) BZDTEST_TEST_ANY_THROW_(expression, BZDTEST_FAIL_FATAL_)
