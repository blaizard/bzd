#include "gtest/gtest.h"
#include "include/container/string.h"

TEST(ContainerString, Empty)
{
	bzd::String<0> null;

    EXPECT_EQ(null.capacity(), 0);
    EXPECT_EQ(null.size(), 0);

	bzd::String<10> test;
	bzd::interface::String iTest = test;

    EXPECT_EQ(iTest.capacity(), 10);
    EXPECT_EQ(iTest.size(), 0);
}

TEST(ContainerString, Base)
{
	bzd::String<10> test("Hello");
	bzd::interface::String iTest = test;

    EXPECT_EQ(iTest.capacity(), 10);
    EXPECT_EQ(iTest.size(), 5);
    EXPECT_STREQ(iTest.data(), "Hello");

	iTest.append(" You");
    EXPECT_EQ(iTest.size(), 9);
    EXPECT_STREQ(iTest.data(), "Hello You");

	iTest.append("!OVERFLOW");
    EXPECT_EQ(iTest.size(), 10);
    EXPECT_STREQ(iTest.data(), "Hello You!");
}

TEST(ContainerString, Resize)
{
	bzd::String<11> test("Hello World");

    EXPECT_EQ(test.capacity(), 11);
    EXPECT_EQ(test.size(), 11);
    EXPECT_STREQ(test.data(), "Hello World");

	test.resize(5);
    EXPECT_EQ(test.size(), 5);
    EXPECT_STREQ(test.data(), "Hello");

	test.resize(0);
    EXPECT_EQ(test.size(), 0);
    EXPECT_STREQ(test.data(), "");
}

TEST(ContainerString, OperatorsPlusEqual)
{
	bzd::String<9> test;

	test += "Hello";
    EXPECT_STREQ(test.data(), "Hello");

	test += " Me";
    EXPECT_STREQ(test.data(), "Hello Me");

	test += "!OVERFLOW";
    EXPECT_STREQ(test.data(), "Hello Me!");
}
