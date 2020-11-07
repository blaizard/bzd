#include "bzd/container/function.h"

#include "cc_test/test.h"

TEST(ContainerFunction, nonCapturingLambda)
{
	static int i = 0;
	bzd::Function<void(void)> callable{[]() { i = 12; }};
	callable();
	EXPECT_EQ(i, 12);
}

TEST(ContainerFunction, capturingLambda)
{
	int j = 0;
	bzd::Function<void(void)> callable{[&j]() { j = -5; }};
	callable();
	EXPECT_EQ(j, -5);
}

static int rawFunctionTestVariable_ = 0;
void rawFunction()
{
	rawFunctionTestVariable_ = 58;
}

TEST(ContainerFunction, rawFunction)
{
	bzd::Function<void(void)> callable{rawFunction};
	callable();
	EXPECT_EQ(rawFunctionTestVariable_, 58);
}
