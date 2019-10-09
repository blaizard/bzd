#include "gtest/gtest.h"
#include "include/container/expected.h"

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
