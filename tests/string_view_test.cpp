#include "bzd/container/string_view.h"

#include "gtest/gtest.h"

TEST(ContainerStringView, Empty)
{
	bzd::StringView str_v = "xyzzy";

	EXPECT_STREQ(str_v.data(), "xyzzy");

	char array[3] = {'B', 'a', 'r'};
	bzd::StringView array_v(array, 3);
}

TEST(ContainerStringView, Constexpr)
{
	constexpr bzd::StringView str_v("xyzzy");
	EXPECT_STREQ(str_v.data(), "xyzzy");
}
