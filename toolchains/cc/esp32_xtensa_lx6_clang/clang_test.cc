#include "cc/bzd/test/test.hh"

TEST_CONSTEXPR_BEGIN(Toolchain, esp32_xtensa_lx6_clang)
{
	EXPECT_TRUE(__clang__);
	EXPECT_EQ(__clang_major__, 12);
	EXPECT_EQ(__clang_minor__, 0);
	EXPECT_EQ(__clang_patchlevel__, 1);
}
TEST_CONSTEXPR_END(Toolchain, esp32_xtensa_lx6_clang)
