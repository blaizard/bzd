#include "gtest/gtest.h"
#include "include/container/string.h"

class A
{
public:
    A() = default;
    int& get()
    {
        return a;
    }

private:
    int a = 2;
};

class B : public A
{
public:
    B() = default;
};

TEST(ContainerString, Empty)
{
	bzd::String<10> test;
	bzd::interface::String& iTest = test;

    EXPECT_EQ(iTest.capacity(), 10);
    EXPECT_EQ(iTest.size(), 0);
}

TEST(ContainerString, Base)
{
	bzd::String<10> test("Hello");
	bzd::interface::String& iTest = test;

    EXPECT_EQ(iTest.capacity(), 10);
    EXPECT_EQ(iTest.size(), 5);
    EXPECT_STREQ(iTest.data(), "Hello");
    EXPECT_EQ(iTest[0], 'H');

	iTest.append(" You");
    EXPECT_EQ(iTest.size(), 9);
    EXPECT_STREQ(iTest.data(), "Hello You");

	iTest.append("!OVERFLOW");
    EXPECT_EQ(iTest.size(), 10);
    EXPECT_STREQ(iTest.data(), "Hello You!");

    iTest.data()[0] = 'W';
    EXPECT_STREQ(iTest.data(), "Wello You!");

    iTest[1] = 'i';
    EXPECT_STREQ(iTest.data(), "Willo You!");
}

TEST(ContainerString, BaseConst)
{
	const bzd::String<10> test("Hello");

    EXPECT_EQ(test.capacity(), 10);
    EXPECT_EQ(test.size(), 5);
    EXPECT_STREQ(test.data(), "Hello");
    EXPECT_EQ(test[0], 'H');
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

TEST(ContainerString, OperatorsEqual)
{
	bzd::String<6> test;

    EXPECT_EQ(test.size(), 0);
    EXPECT_STREQ(test.data(), "");

	test = "Hello";
    EXPECT_EQ(test.size(), 5);
    EXPECT_STREQ(test.data(), "Hello");

	test = "Hello World";
    EXPECT_EQ(test.size(), 6);
    EXPECT_STREQ(test.data(), "Hello ");
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

TEST(ContainerString, Constexpr)
{
    static constexpr bzd::String<6> test("Hello");
    EXPECT_STREQ(test.data(), "Hello");
}

TEST(ContainerString, Construct)
{
    bzd::StringView str = "xyzzy";
    bzd::String<6> test(str);
	EXPECT_STREQ(test.data(), "xyzzy");
}

TEST(ContainerString, Copy)
{
    bzd::String<6> test("Hello");
	EXPECT_STREQ(test.data(), "Hello");

    bzd::String<6> copy(test);
	EXPECT_STREQ(copy.data(), "Hello");

    bzd::String<6> copy2 = test;
	EXPECT_STREQ(copy2.data(), "Hello");
}

TEST(ContainerString, Reverse)
{
    {
        bzd::String<6> test("Hello");
        test.reverse();
        EXPECT_STREQ(test.data(), "olleH");
    }
    {
        bzd::String<6> test("Hell");
        test.reverse();
        EXPECT_STREQ(test.data(), "lleH");
    }
    {
        bzd::String<6> test("");
        test.reverse();
        EXPECT_STREQ(test.data(), "");
    }
    {
        bzd::String<6> test("a");
        test.reverse();
        EXPECT_STREQ(test.data(), "a");
    }
}
