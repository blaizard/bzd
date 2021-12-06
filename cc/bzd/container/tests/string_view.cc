#include "cc/bzd/container/string_view.hh"

#include "cc/bzd/test/test.hh"

TEST_CONSTEXPR_BEGIN(ContainerStringView, Constexpr)
{
	constexpr bzd::StringView str_v("xyzzy");
	EXPECT_STREQ(str_v.data(), "xyzzy");
}
TEST_CONSTEXPR_END(ContainerStringView, Constexpr)
