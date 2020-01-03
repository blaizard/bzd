#include "bzd/container/constexpr_string_view.h"

#include "gtest/gtest.h"

TEST(ContainerConstexprStringView, base)
{
	constexpr auto strObj = CONSTEXPR_STRING_VIEW("Hello");
	static_assert(strObj.data()[0] == 'H', "test");
}
