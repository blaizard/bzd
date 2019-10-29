#include "gtest/gtest.h"
#include "include/container/constexpr_string_view.h"

TEST(ContainerConstexprStringView, base)
{
	constexpr auto strObj = CONSTEXPR_STRING_VIEW("Hello");
	static_assert(strObj.data()[0] == 'H', "test");
}
