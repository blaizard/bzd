#include "gtest/gtest.h"
#include "bzd/container/expected.h"

TEST(ContainerExpected, returnVoid)
{
	auto fct = [](const bool makeError) -> bzd::Expected<void, std::string> {
		if (makeError) return bzd::makeUnexpected("KO");
    	return {};
	};

	const auto ret = fct(/*makeError*/true);
    EXPECT_FALSE(ret);
    EXPECT_STREQ(ret.error().c_str(), "KO");
}

TEST(ContainerExpected, returnReference)
{
	int data = 0;
	auto fct = [&](const bool makeError) -> bzd::Expected<int&, std::string> {
		if (makeError) return bzd::makeUnexpected("KO");
		data = 42;
    	return data;
	};
	const auto ret = fct(/*makeError*/false);
    EXPECT_EQ(*ret, 42);
	data = 37;
	EXPECT_EQ(*ret, 37);
}

TEST(ContainerExpected, returnPointer)
{
	int data = 0;
	auto fct = [&](const bool makeError) -> bzd::Expected<int*, std::string> {
		if (makeError) return bzd::makeUnexpected("KO");
		data = 42;
    	return &data;
	};
	const auto ret = fct(/*makeError*/false);
    EXPECT_EQ(*(*ret), 42);
	data = 37;
    EXPECT_EQ(*(*ret), 37);
}
