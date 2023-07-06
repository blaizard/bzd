#include "cc/bzd/test/test.hh"

TEST_CONSTEXPR_BEGIN(Toolchain, linux_x86_64_gcc)
{
	EXPECT_TRUE(__GNUC__ && __cplusplus);
	EXPECT_EQ(__GNUC__, 13);
	EXPECT_EQ(__GNUC_MINOR__, 1);
	EXPECT_EQ(__GNUC_PATCHLEVEL__, 0);
}
TEST_CONSTEXPR_END(Toolchain, linux_x86_64_gcc)
