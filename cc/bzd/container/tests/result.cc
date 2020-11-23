#include "bzd/container/result.h"

#include "cc_test/test.h"

#include <string>

TEST(ContainerResult, returnVoid)
{
	auto fct = [](const bool makeError) -> bzd::Result<void, std::string> {
		if (makeError) return bzd::makeError("KO");
		return bzd::nullresult;
	};

	const auto ret = fct(/*makeError*/ true);
	EXPECT_FALSE(ret);
	EXPECT_STREQ(ret.error().c_str(), "KO");
}

TEST(ContainerResult, returnReference)
{
	int data = 0;
	auto fct = [&](const bool makeError) -> bzd::Result<int&, std::string> {
		if (makeError) return bzd::makeError("KO");
		data = 42;
		return data;
	};
	const auto ret = fct(/*makeError*/ false);
	EXPECT_EQ(*ret, 42);
	data = 37;
	EXPECT_EQ(*ret, 37);
}

TEST(ContainerResult, returnPointer)
{
	int data = 0;
	auto fct = [&](const bool makeError) -> bzd::Result<int*, std::string> {
		if (makeError) return bzd::makeError("KO");
		data = 42;
		return &data;
	};
	const auto ret = fct(/*makeError*/ false);
	EXPECT_EQ(*(*ret), 42);
	data = 37;
	EXPECT_EQ(*(*ret), 37);
}
