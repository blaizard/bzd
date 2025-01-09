#include "cc/targets/tests/timer.hh"

#include "cc/bzd/test/test.hh"

TEST_ASYNC(Timer, Basic)
{
	auto& timer = test.template getRunner<tests::CustomRunner>().timer;
	co_await !timer.delay(bzd::units::Millisecond{10});

	co_return {};
}

TEST_ASYNC(Timer, Delay)
{
	auto& timer = test.template getRunner<tests::CustomRunner>().timer;
	bzd::Atomic<bzd::Size> nbYields{0u};
	auto yieldCounter = [&nbYields]() -> bzd::Async<> {
		while (true)
		{
			++nbYields;
			co_await bzd::async::yield();
		}
		co_return {};
	};

	// Check that it yields at least several times.
	co_await !bzd::async::any(timer.delay(bzd::units::Millisecond{10}), yieldCounter());
	EXPECT_GT(nbYields.load(), 0u);

	co_return {};
}

TEST_ASYNC(Timer, Timeout)
{
	auto& timer = test.template getRunner<tests::CustomRunner>().timer;

	// Check that the timeout works as expected.
	const auto result = co_await bzd::async::any(timer.delay(bzd::units::Second{1000}),
												 timer.timeout(bzd::units::Millisecond{10}),
												 timer.delay(bzd::units::Second{1000}));
	EXPECT_TRUE(result.template get<0>().empty());
	EXPECT_TRUE(result.template get<1>().hasValue());
	const auto& value = result.template get<1>().value();
	EXPECT_FALSE(value.hasValue());

	co_return {};
}

TEST_ASYNC(Timer, MultiDelay)
{
	auto& timer = test.template getRunner<tests::CustomRunner>().timer;

	for (int i = 0; i < 10; ++i)
	{
		// Check that multiple delay can be called simultaneously.
		const auto result = co_await bzd::async::all(timer.delay(bzd::units::Millisecond{test.template random<int, 1, 10>()}),
													 timer.delay(bzd::units::Millisecond{test.template random<int, 1, 10>()}),
													 timer.delay(bzd::units::Millisecond{test.template random<int, 1, 10>()}),
													 timer.delay(bzd::units::Millisecond{test.template random<int, 1, 10>()}),
													 timer.delay(bzd::units::Millisecond{test.template random<int, 1, 10>()}),
													 timer.delay(bzd::units::Millisecond{test.template random<int, 1, 10>()}),
													 timer.delay(bzd::units::Millisecond{test.template random<int, 1, 10>()}),
													 timer.delay(bzd::units::Millisecond{test.template random<int, 1, 10>()}),
													 timer.delay(bzd::units::Millisecond{test.template random<int, 1, 10>()}),
													 timer.delay(bzd::units::Millisecond{test.template random<int, 1, 10>()}));
		EXPECT_TRUE(result.template get<0>().hasValue());
		EXPECT_TRUE(result.template get<1>().hasValue());
		EXPECT_TRUE(result.template get<2>().hasValue());
		EXPECT_TRUE(result.template get<3>().hasValue());
		EXPECT_TRUE(result.template get<4>().hasValue());
		EXPECT_TRUE(result.template get<5>().hasValue());
		EXPECT_TRUE(result.template get<6>().hasValue());
		EXPECT_TRUE(result.template get<7>().hasValue());
		EXPECT_TRUE(result.template get<8>().hasValue());
		EXPECT_TRUE(result.template get<9>().hasValue());
	}

	co_return {};
}
