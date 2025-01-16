#include "cc/bzd/test/test.hh"
#include "cc/bzd/test/types/generator.hh"
#include "cc/bzd/test/types/ichannel.hh"
#include "cc/bzd/utility/ranges/views_async/views_async.hh"

TEST_ASYNC(AsyncRange, Drop)
{
	auto gen = bzd::test::generator(5);
	auto view = gen | bzd::ranges::drop(2);

	const auto expected = {2, 3, 4};
	EXPECT_EQ_RANGE(view, expected);

	co_return {};
}

TEST_ASYNC(AsyncRange, Join)
{
	const auto data = {"Hello"_sv, " "_sv, "World"_sv};
	auto gen = bzd::test::generator(data);
	auto view = gen | bzd::ranges::join();

	EXPECT_EQ_RANGE(view, "Hello World"_sv);

	co_return {};
}
