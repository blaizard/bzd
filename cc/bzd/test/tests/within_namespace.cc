#include "cc/bzd/test/test.hh"

namespace custom::random::name::space {

TEST(Test, Sync) { EXPECT_TRUE(true); }

TEST_ASYNC(Test, Async) { co_return {}; }

TEST_CONSTEXPR_BEGIN(Test, Constexpr)
{
	constexpr bzd::Bool value{true};
	EXPECT_TRUE(value);
}
TEST_CONSTEXPR_END(Test, Constexpr)

} // namespace custom::random::name::space
