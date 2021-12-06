#include "cc/bzd/test/test.hh"

TEST(Toolchain, linux_x86_64_clang)
{
	EXPECT_TRUE(__clang__);
	EXPECT_EQ(__clang_major__, 12);
	EXPECT_EQ(__clang_minor__, 0);
	EXPECT_EQ(__clang_patchlevel__, 0);
}
