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
		context.setContinuation(continuation);
		auto maybeContinuation = context.popContinuation();
		EXPECT_TRUE(maybeContinuation);
		EXPECT_EQ(&maybeContinuation.value(), &executable);
		maybeContinuation = context.popContinuation();
		EXPECT_FALSE(maybeContinuation);
	}

	{
		auto callback = [&]() -> bzd::Optional<Executable&> { return executable; };
		Continuation continuation{OnTerminateCallback{callback}};
		context.setContinuation(continuation);
		auto maybeContinuation = context.popContinuation();
		EXPECT_TRUE(maybeContinuation);
		EXPECT_EQ(&maybeContinuation.value(), &executable);
		maybeContinuation = context.popContinuation();
		EXPECT_FALSE(maybeContinuation);
	}

	{
		auto callback = []() -> bzd::Optional<Executable&> { return bzd::nullopt; };
		Continuation continuation{OnTerminateCallback{callback}};
		context.setContinuation(continuation);
		auto maybeContinuation = context.popContinuation();
		EXPECT_FALSE(maybeContinuation);
	}
}
