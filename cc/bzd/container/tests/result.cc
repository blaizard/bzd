#include "bzd/container/result.h"

#include "cc_test/test.h"

#include <string>

constexpr bzd::Result<void, int> constexprResultFct(const bool makeError)
{
	if (makeError)
	{
		return bzd::makeError(42);
	}
	return bzd::nullresult;
}

TEST(ContainerResult, constexprType)
{
	constexpr auto ret = constexprResultFct(true); // makeError
	EXPECT_FALSE(ret);
	EXPECT_EQ(ret.error(), 42);
}

TEST(ContainerResult, returnVoid)
{
	auto fct = [](const bool makeError) -> bzd::Result<void, std::string> {
		if (makeError) return bzd::makeError("KO");
		return bzd::nullresult;
	};

	const auto ret = fct(true); // makeError
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
	const auto ret = fct(false); // makeError
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
	const auto ret = fct(false); // makeError
	EXPECT_EQ(*(*ret), 42);
	data = 37;
	EXPECT_EQ(*(*ret), 37);
}

TEST(ContainerResult, constructor)
{
	// value copied
	{
		bzd::Result<int, const char*> result{12};
		EXPECT_TRUE(result);
		EXPECT_EQ(*result, 12);

		// Only move constructors are possible
		bzd::Result<int, const char*> resultMoved{bzd::move(result)};
		EXPECT_TRUE(resultMoved);
		EXPECT_EQ(*resultMoved, 12);
	}

	// error copied
	{
		bzd::Result<int, const char*> result{bzd::makeError("ha")};
		EXPECT_FALSE(result);
		EXPECT_STREQ(result.error(), "ha");

		// Only move constructors are possible
		bzd::Result<int, const char*> resultMoved{bzd::move(result)};
		EXPECT_FALSE(resultMoved);
		EXPECT_STREQ(resultMoved.error(), "ha");
	}
}
