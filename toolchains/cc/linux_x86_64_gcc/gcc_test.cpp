#include "cc_test/test.h"

TEST(Toolchain, linux_x86_64_gcc)
{
	EXPECT_TRUE(__GNUC__ && __cplusplus);
	EXPECT_EQ(__GNUC__, 8);
	EXPECT_EQ(__GNUC_MINOR__, 3);
	EXPECT_EQ(__GNUC_PATCHLEVEL__, 0);
}
