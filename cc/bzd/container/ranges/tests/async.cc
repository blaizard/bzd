#include "cc/bzd/container/ichannel_buffered.hh"
#include "cc/bzd/container/ranges/views_async/views_async.hh"
#include "cc/bzd/test/test.hh"
#include "cc/bzd/test/types/generator.hh"
#include "cc/bzd/test/types/ichannel.hh"

TEST_ASYNC(AsyncRange, Drop)
{
	auto gen = bzd::test::generator(5);
	auto view = gen | bzd::ranges::drop(2);

	const auto expected = {2, 3, 4};
	EXPECT_EQ_RANGE(view, expected);

	co_return {};
}
