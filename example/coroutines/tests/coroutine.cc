#include "example/coroutines/async.h"

#include "cc_test/test.h"

bzd::Async nop()
{
	co_return 42;
}

TEST(Coroutine, Base)
{
	auto promise = nop();
	promise.sync();
}

bzd::Async nested()
{
	auto result = co_await nop();
	co_return result;
}

TEST(Coroutine, Nested)
{
	auto promise = nested();
	promise.sync();
}
