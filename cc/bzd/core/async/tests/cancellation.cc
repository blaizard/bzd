#include "cc/bzd/core/async/cancellation.hh"

#include "cc/bzd/test/test.hh"

TEST(Cancellation, Simple)
{
	bzd::CancellationToken token{};

	EXPECT_FALSE(token.isCanceled());
	token.trigger();
	EXPECT_TRUE(token.isCanceled());
}

TEST(Cancellation, AttachedTriggerParent)
{
	bzd::CancellationToken parent{};
	bzd::CancellationToken token{};

	token.attachTo(parent);
	EXPECT_FALSE(token.isCanceled());
	EXPECT_FALSE(parent.isCanceled());
	parent.trigger();
	EXPECT_TRUE(token.isCanceled());
	EXPECT_TRUE(parent.isCanceled());
}

TEST(Cancellation, AttachedTriggerChild)
{
	bzd::CancellationToken parent{};
	bzd::CancellationToken token{};

	token.attachTo(parent);
	EXPECT_FALSE(token.isCanceled());
	EXPECT_FALSE(parent.isCanceled());
	token.trigger();
	EXPECT_TRUE(token.isCanceled());
	EXPECT_FALSE(parent.isCanceled());
}
