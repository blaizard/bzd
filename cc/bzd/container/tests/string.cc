#include "cc/bzd/container/string.hh"

#include "cc/bzd/test/test.hh"

class A
{
public:
	A() = default;
	int& get() { return a; }

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
	bzd::String<10> string;
	bzd::interface::String& iString = string;

	EXPECT_EQ(iString.capacity(), 10U);
	EXPECT_EQ(iString.size(), 0U);
}

TEST(ContainerString, Base)
{
	bzd::String<10> string("Hello");
	bzd::interface::String& iString = string;

	EXPECT_EQ(iString.capacity(), 10U);
	EXPECT_EQ(iString.size(), 5U);
	EXPECT_STREQ(iString.data(), "Hello");
	EXPECT_EQ(iString[0], 'H');

	iString.append(" You");
	EXPECT_EQ(iString.size(), 9U);
	EXPECT_STREQ(iString.data(), "Hello You");

	iString.append("!OVERFLOW");
	EXPECT_EQ(iString.size(), 10U);
	EXPECT_STREQ(iString.data(), "Hello You!");

	iString.data()[0] = 'W';
	EXPECT_STREQ(iString.data(), "Wello You!");

	iString[1] = 'i';
	EXPECT_STREQ(iString.data(), "Willo You!");
}

TEST(ContainerString, Append)
{
	{
		bzd::String<10> string{};
		string += "hello";
		EXPECT_STREQ(string.data(), "hello");
		EXPECT_EQ(string, "hello");
		string += "s";
		EXPECT_STREQ(string.data(), "hellos");
		EXPECT_EQ(string, "hellos");
		string += 's';
		EXPECT_STREQ(string.data(), "helloss");
		EXPECT_EQ(string, "helloss");
		string += "me"_sv;
		EXPECT_STREQ(string.data(), "hellossme");
		EXPECT_EQ(string, "hellossme");
	}
}

TEST(ContainerString, BaseConst)
{
	const bzd::String<10> string("Hello");

	EXPECT_EQ(string.capacity(), 10U);
	EXPECT_EQ(string.size(), 5U);
	EXPECT_STREQ(string.data(), "Hello");
	EXPECT_EQ(string[0], 'H');
}

TEST(ContainerString, Resize)
{
	bzd::String<11> string("Hello World");

	EXPECT_EQ(string.capacity(), 11U);
	EXPECT_EQ(string.size(), 11U);
	EXPECT_STREQ(string.data(), "Hello World");

	string.resize(5);
	EXPECT_EQ(string.size(), 5U);
	EXPECT_STREQ(string.data(), "Hello");

	string.resize(0);
	EXPECT_EQ(string.size(), 0U);
	EXPECT_STREQ(string.data(), "");
}

TEST(ContainerString, OperatorsEqual)
{
	bzd::String<6> string;

	EXPECT_EQ(string.size(), 0U);
	EXPECT_STREQ(string.data(), "");

	string = "Hello";
	EXPECT_EQ(string.size(), 5U);
	EXPECT_STREQ(string.data(), "Hello");

	string = "Hello World";
	EXPECT_EQ(string.size(), 6U);
	EXPECT_STREQ(string.data(), "Hello ");
}

TEST(ContainerString, OperatorsPlusEqual)
{
	bzd::String<9> string;

	string += "Hello";
	EXPECT_STREQ(string.data(), "Hello");

	string += " Me";
	EXPECT_STREQ(string.data(), "Hello Me");

	string += "!OVERFLOW";
	EXPECT_STREQ(string.data(), "Hello Me!");
}

TEST_CONSTEXPR_BEGIN(ContainerString, Constexpr)
{
	bzd::String<6> string("Hello");
	EXPECT_STREQ(string.data(), "Hello");
}
TEST_CONSTEXPR_END(ContainerString, Constexpr)

TEST(ContainerString, Construct)
{
	bzd::StringView str = "xyzzy";
	bzd::String<6> string(str);
	EXPECT_STREQ(string.data(), "xyzzy");
}

TEST(ContainerString, Copy)
{
	bzd::String<6> string("Hello");
	EXPECT_STREQ(string.data(), "Hello");

	bzd::String<6> copy(string);
	EXPECT_STREQ(copy.data(), "Hello");

	bzd::String<6> copy2 = string;
	EXPECT_STREQ(copy2.data(), "Hello");
}
