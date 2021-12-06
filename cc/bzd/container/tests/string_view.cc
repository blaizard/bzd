#include "cc/bzd/container/string_view.hh"

#include "cc/bzd/test/test.hh"

TEST(ContainerStringView, Constexpr)
{
	constexpr bzd::StringView str_v("xyzzy");
	EXPECT_STREQ(str_v.data(), "xyzzy");
}
