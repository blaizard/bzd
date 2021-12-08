#pragma once

#include "cc/bzd/test/test_internal.hh"

/// Defines an individual test named testCaseName in the test suite testName, consisting of the given statements.
/// The statements within the test body can be any code under test. Assertions used within the test body determine the outcome of the test.
#define TEST(testCaseName, testName) BZDTEST_(testCaseName, testName)

/// Run a coroutine-based test in an asynchronous context.
#define TEST_ASYNC(testCaseName, testName) BZDTEST_ASYNC_(testCaseName, testName)

/// Executes a test case at compile time.
/// \{
#define TEST_CONSTEXPR_BEGIN(testCaseName, testName) BZDTEST_CONSTEXPR_BEGIN_(testCaseName, testName)
#define TEST_CONSTEXPR_END(testCaseName, testName) BZDTEST_CONSTEXPR_END_(testCaseName, testName)
/// \}

/// Verifies that condition is true.
/// \{
#define EXPECT_TRUE(condition) BZDTEST_TEST_BOOLEAN_(condition, condition, true, BZDTEST_FAIL_NONFATAL_)
#define ASSERT_TRUE(condition) BZDTEST_TEST_BOOLEAN_(condition, condition, true, BZDTEST_FAIL_FATAL_)
/// \}

/// Verifies that condition is false.
/// \{
#define EXPECT_FALSE(condition) BZDTEST_TEST_BOOLEAN_(!(condition), condition, false, BZDTEST_FAIL_NONFATAL_)
#define ASSERT_FALSE(condition) BZDTEST_TEST_BOOLEAN_(!(condition), condition, false, BZDTEST_FAIL_FATAL_)
/// \}

/// Verifies that expression1 == expression2.
/// \{
#define EXPECT_EQ(expression1, expression2) BZDTEST_TEST_EQ_(expression1, expression2, BZDTEST_FAIL_NONFATAL_)
#define ASSERT_EQ(expression1, expression2) BZDTEST_TEST_EQ_(expression1, expression2, BZDTEST_FAIL_FATAL_)
/// \}

/// Verifies that expression1 != expression2.
/// \{
#define EXPECT_NE(expression1, expression2) BZDTEST_TEST_NE_(expression1, expression2, BZDTEST_FAIL_NONFATAL_)
#define ASSERT_NE(expression1, expression2) BZDTEST_TEST_NE_(expression1, expression2, BZDTEST_FAIL_FATAL_)
/// \}

/// Verifies that expression1 < expression2.
/// \{
#define EXPECT_LT(expression1, expression2) BZDTEST_TEST_LT_(expression1, expression2, BZDTEST_FAIL_NONFATAL_)
#define ASSERT_LT(expression1, expression2) BZDTEST_TEST_LT_(expression1, expression2, BZDTEST_FAIL_FATAL_)
/// \}

/// Verifies that expression1 <= expression2.
/// \{
#define EXPECT_LE(expression1, expression2) BZDTEST_TEST_LE_(expression1, expression2, BZDTEST_FAIL_NONFATAL_)
#define ASSERT_LE(expression1, expression2) BZDTEST_TEST_LE_(expression1, expression2, BZDTEST_FAIL_FATAL_)
/// \}

/// Verifies that expression1 > expression2.
/// \{
#define EXPECT_GT(expression1, expression2) BZDTEST_TEST_GT_(expression1, expression2, BZDTEST_FAIL_NONFATAL_)
#define ASSERT_GT(expression1, expression2) BZDTEST_TEST_GT_(expression1, expression2, BZDTEST_FAIL_FATAL_)
/// \}

/// Verifies that expression1 >= expression2.
/// \{
#define EXPECT_GE(expression1, expression2) BZDTEST_TEST_GE_(expression1, expression2, BZDTEST_FAIL_NONFATAL_)
#define ASSERT_GE(expression1, expression2) BZDTEST_TEST_GE_(expression1, expression2, BZDTEST_FAIL_FATAL_)
/// \}

/// Verifies that the difference between number1 and number2 does not exceed the absolute error bound absError.
/// \{
#define EXPECT_NEAR(number1, number2, absError) BZDTEST_TEST_NEAR_(number1, number2, absError, BZDTEST_FAIL_NONFATAL_)
#define ASSERT_NEAR(number1, number2, absError) BZDTEST_TEST_NEAR_(number1, number2, absError, BZDTEST_FAIL_FATAL_)
/// \}

/// Verifies that the two C strings str1 and str2 have the same contents.
/// \{
#define EXPECT_STREQ(str1, str2) BZDTEST_TEST_STREQ_(str1, str2, BZDTEST_FAIL_NONFATAL_)
#define ASSERT_STREQ(str1, str2) BZDTEST_TEST_STREQ_(str1, str2, BZDTEST_FAIL_FATAL_)
/// \}

/// Verifies that statement throws an exception of any type.
/// \{
#define EXPECT_ANY_THROW(expression) BZDTEST_TEST_ANY_THROW_(expression, BZDTEST_FAIL_NONFATAL_)
#define ASSERT_ANY_THROW(expression) BZDTEST_TEST_ANY_THROW_(expression, BZDTEST_FAIL_FATAL_)
/// \}
