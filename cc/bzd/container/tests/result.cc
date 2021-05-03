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
	EXPECT_EQ(ret.value(), 42);
	data = 37;
	EXPECT_EQ(ret.value(), 37);
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
	EXPECT_EQ(*(ret.value()), 42);
	data = 37;
	EXPECT_EQ(*(ret.value()), 37);
}

TEST(ContainerResult, constructor)
{
	// value copied
	bzd::Result<int, const char*> resultValue{12};
	EXPECT_TRUE(resultValue);
	EXPECT_EQ(resultValue.value(), 12);

	// Copy constructors
	bzd::Result<int, const char*> resultValueCopied{resultValue};
	EXPECT_TRUE(resultValueCopied);
	EXPECT_EQ(resultValueCopied.value(), 12);

	// Move constructors
	bzd::Result<int, const char*> resultValueMoved{bzd::move(resultValue)};
	EXPECT_TRUE(resultValueMoved);
	EXPECT_EQ(resultValueMoved.value(), 12);

	// error copied
	bzd::Result<int, const char*> resultError{bzd::makeError("ha")};
	EXPECT_FALSE(resultError);
	EXPECT_STREQ(resultError.error(), "ha");

	// Move constructors
	bzd::Result<int, const char*> resultErrorMoved{bzd::move(resultError)};
	EXPECT_FALSE(resultErrorMoved);
	EXPECT_STREQ(resultErrorMoved.error(), "ha");

	// Move assignment
	resultValue = bzd::move(resultErrorMoved);
	EXPECT_FALSE(resultValue);
	EXPECT_STREQ(resultValue.error(), "ha");

	// Move assignment
	resultError = bzd::move(resultValue);
	EXPECT_FALSE(resultError);
	EXPECT_STREQ(resultError.error(), "ha");

	// Move void
	bzd::Result<void, bool> resultVoid{bzd::nullresult};
	EXPECT_TRUE(resultVoid);
	bzd::Result<void, bool> resultVoidMoved = bzd::move(resultVoid);
	EXPECT_TRUE(resultVoidMoved);
}
