#include "cc/bzd/core/async/executor.hh"

#include "cc/bzd/test/test.hh"

class Executable : public bzd::interface::Executable<Executable>
{
};

TEST(ExecutorContext, Continuation)
{
	bzd::ExecutorContext<Executable> context{};
	using Continuation = bzd::ExecutorContext<Executable>::Continuation;
	using OnTerminateCallback = bzd::ExecutorContext<Executable>::OnTerminateCallback;
	Executable executable{};

	{
		auto maybeContinuation = context.popContinuation();
		EXPECT_FALSE(maybeContinuation);
	}

	{
		Continuation continuation{&executable};
		context.setContinuation(bzd::move(continuation));
		auto maybeContinuation = context.popContinuation();
		EXPECT_TRUE(maybeContinuation);
		EXPECT_EQ(&maybeContinuation.value(), &executable);
		maybeContinuation = context.popContinuation();
		EXPECT_FALSE(maybeContinuation);
	}

	{
		auto callback = [&]() -> bzd::Optional<Executable&> { return executable; };
		Continuation continuation{OnTerminateCallback{callback}};
		context.setContinuation(bzd::move(continuation));
		auto maybeContinuation = context.popContinuation();
		EXPECT_TRUE(maybeContinuation);
		EXPECT_EQ(&maybeContinuation.value(), &executable);
		maybeContinuation = context.popContinuation();
		EXPECT_FALSE(maybeContinuation);
	}

	{
		auto callback = []() -> bzd::Optional<Executable&> { return bzd::nullopt; };
		Continuation continuation{OnTerminateCallback{callback}};
		context.setContinuation(bzd::move(continuation));
		auto maybeContinuation = context.popContinuation();
		EXPECT_FALSE(maybeContinuation);
	}
}

/*
bzd::Async<bzd::impl::AsyncExecutor*> fetchExecutor()
{
	auto exec = co_await bzd::async::getExecutor();
	co_return exec;
}

TEST(Coroutine, Executor)
{
	bzd::impl::AsyncExecutor executor;
	auto promise = fetchExecutor();
	const auto result = promise.run(executor);
	EXPECT_TRUE(result);
	EXPECT_EQ(result.value(), &executor);
}
*/
