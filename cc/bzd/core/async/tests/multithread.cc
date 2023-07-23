#include "cc/bzd/test/multithread.hh"

#include "cc/bzd/container/array.hh"
#include "cc/bzd/core/async.hh"

enum class ForkType
{
	any,
	all,
	random
};

template <class Result>
[[nodiscard]] bool isValidResultForAll(Result&& result)
{
	return bzd::apply([](auto&... asyncs) -> bool { return (asyncs.hasValue() && ...); }, bzd::forward<Result>(result));
}

template <class Result>
[[nodiscard]] bool isValidResultForAny(Result&& result)
{
	return bzd::apply([](auto&... asyncs) -> bool { return (asyncs.hasValue() || ...); }, bzd::forward<Result>(result));
}

bzd::Async<> cancellationWorkload(const bzd::Size counter)
{
	for (bzd::Size current = 0; current < counter; ++current)
	{
		co_await bzd::async::yield();
	}
	co_return {};
}

template <ForkType forkType>
bzd::Async<> cancellationNestedWorkload(const bzd::Size counter)
{
	for (bzd::Size current = 0; current < counter; ++current)
	{
		auto promise1 = cancellationWorkload(counter);
		auto promise2 = cancellationNestedWorkload<forkType>(counter / 2);
		auto promise3 = cancellationNestedWorkload<forkType>(counter / 3);
		if constexpr (forkType == ForkType::any)
		{
			auto result = co_await bzd::async::any(bzd::move(promise1), bzd::move(promise2), bzd::move(promise3));
			EXPECT_TRUE(isValidResultForAny(result));
		}
		else if constexpr (forkType == ForkType::all)
		{
			auto result = co_await bzd::async::all(bzd::move(promise1), bzd::move(promise2), bzd::move(promise3));
			EXPECT_TRUE(isValidResultForAll(result));
		}
		else if constexpr (forkType == ForkType::random)
		{
			if (counter % 2)
			{
				auto result = co_await bzd::async::any(bzd::move(promise1), bzd::move(promise2), bzd::move(promise3));
				EXPECT_TRUE(isValidResultForAny(result));
			}
			else
			{
				auto result = co_await bzd::async::all(bzd::move(promise1), bzd::move(promise2), bzd::move(promise3));
				EXPECT_TRUE(isValidResultForAll(result));
			}
		}
		else
		{
			bzd::assert::unreachable();
		}
	}
	co_return {};
}

template <ForkType forkType, class Function>
bzd::Async<> runConcurrentTest(bzd::Async<> (*workload)(const bzd::Size), const bzd::Size iterations, const Function counterGenerator)
{
	for (bzd::Size iteration = 0; iteration < iterations; ++iteration)
	{
		auto promise1 = workload(counterGenerator());
		auto promise2 = workload(counterGenerator());
		auto promise3 = workload(counterGenerator());
		auto promise4 = workload(counterGenerator());
		auto promise5 = workload(counterGenerator());

		if constexpr (forkType == ForkType::any)
		{
			auto result = co_await bzd::async::any(bzd::move(promise1),
												   bzd::move(promise2),
												   bzd::move(promise3),
												   bzd::move(promise4),
												   bzd::move(promise5));
			EXPECT_TRUE(isValidResultForAny(result));
		}
		else if constexpr (forkType == ForkType::all)
		{
			auto result = co_await bzd::async::all(bzd::move(promise1),
												   bzd::move(promise2),
												   bzd::move(promise3),
												   bzd::move(promise4),
												   bzd::move(promise5));
			EXPECT_TRUE(isValidResultForAll(result));
		}
		else if constexpr (forkType == ForkType::random)
		{
			if (static_cast<bzd::Bool>(counterGenerator() % 2))
			{
				auto result = co_await bzd::async::any(bzd::move(promise1),
													   bzd::move(promise2),
													   bzd::move(promise3),
													   bzd::move(promise4),
													   bzd::move(promise5));
				EXPECT_TRUE(isValidResultForAny(result));
			}
			else
			{
				auto result = co_await bzd::async::all(bzd::move(promise1),
													   bzd::move(promise2),
													   bzd::move(promise3),
													   bzd::move(promise4),
													   bzd::move(promise5));
				EXPECT_TRUE(isValidResultForAll(result));
			}
		}
		else
		{
			bzd::assert::unreachable();
		}
	}

	co_return {};
}

TEST_ASYNC_MULTITHREAD(Coroutine, StressAllNull, 10)
{
	co_await !runConcurrentTest<ForkType::all>(cancellationWorkload, 1000, []() { return 0; });
	co_return {};
}

TEST_ASYNC_MULTITHREAD(Coroutine, StressAnyNull, 10)
{
	co_await !runConcurrentTest<ForkType::any>(cancellationWorkload, 1000, []() { return 0; });
	co_return {};
}

TEST_ASYNC_MULTITHREAD(Coroutine, StressAllFixed, 10)
{
	co_await !runConcurrentTest<ForkType::all>(cancellationWorkload, 1000, []() { return 10; });
	co_return {};
}

TEST_ASYNC_MULTITHREAD(Coroutine, StressAnyFixed, 10)
{
	co_await !runConcurrentTest<ForkType::any>(cancellationWorkload, 1000, []() { return 10; });
	co_return {};
}

TEST_ASYNC_MULTITHREAD(Coroutine, StressAllRandom, 10)
{
	co_await !runConcurrentTest<ForkType::all>(cancellationWorkload, 1000, [&]() { return test.template random<int, 0, 100>(); });
	co_return {};
}

TEST_ASYNC_MULTITHREAD(Coroutine, StressAnyRandom, 10)
{
	co_await !runConcurrentTest<ForkType::any>(cancellationWorkload, 1000, [&]() { return test.template random<int, 0, 100>(); });
	co_return {};
}

TEST_ASYNC_MULTITHREAD(Coroutine, StressAllNested, 10)
{
	co_await !runConcurrentTest<ForkType::all>(cancellationNestedWorkload<ForkType::all>, 100, [&]() {
		return test.template random<int, 0, 5>();
	});
	co_return {};
}

TEST_ASYNC_MULTITHREAD(Coroutine, StressAnyNested, 10)
{
	co_await !runConcurrentTest<ForkType::any>(cancellationNestedWorkload<ForkType::any>, 100, [&]() {
		return test.template random<int, 0, 5>();
	});
	co_return {};
}

TEST_ASYNC_MULTITHREAD(Coroutine, StressAllAnyNested, 10)
{
	co_await !runConcurrentTest<ForkType::all>(cancellationNestedWorkload<ForkType::any>, 100, [&]() {
		return test.template random<int, 0, 5>();
	});
	co_return {};
}

TEST_ASYNC_MULTITHREAD(Coroutine, StressAnyAllNested, 10)
{
	co_await !runConcurrentTest<ForkType::any>(cancellationNestedWorkload<ForkType::all>, 100, [&]() {
		return test.template random<int, 0, 5>();
	});
	co_return {};
}

TEST_ASYNC_MULTITHREAD(Coroutine, StressRandomNested, 10)
{
	co_await !runConcurrentTest<ForkType::random>(cancellationNestedWorkload<ForkType::random>, 100, [&]() {
		return test.template random<int, 0, 5>();
	});
	co_return {};
}
