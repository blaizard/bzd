#include "bzd/container/string_constexpr.h"

#include "cc_test/test.h"

TEST(ContainerConstexprStringView, base)
{
	constexpr auto strObj = CSTR("Hello");
	static_assert(strObj.data()[0] == 'H', "test");
}
