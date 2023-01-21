#pragma once

#include "cc/bzd.hh"
#include "example/io/simple/composition.hh"

namespace example {

template <class Context>
class Emitter
{
public:
	constexpr explicit Emitter(Context& context) noexcept : context_{context} {}

	bzd::Async<> generate() noexcept
	{
		bzd::Int32 counter = 0;
		while (true)
		{
			context_.io.send.set(counter);
			co_await !bzd::print("Sending {:d}\n"_csv, counter);
			co_await !bzd::delay(100_ms);
			++counter;
		}
		co_return {};
	}

private:
	Context& context_;
};

template <class Context>
class Poller
{
public:
	constexpr explicit Poller(Context& context) noexcept : context_{context} {}

	bzd::Async<> poll() noexcept
	{
		bzd::Int32 counter = 0;
		while (counter < 10)
		{
			const auto result = co_await !context_.io.receive.get();
			co_await !bzd::print("Received {:}\n"_csv, result.value());
			++counter;
		}
		co_return {};
	}

private:
	Context& context_;
};

} // namespace example