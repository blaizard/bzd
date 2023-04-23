#pragma once

#include "cc/bzd.hh"
#include "example/system/composition.hh"

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
			co_await !context_.io.send.set(counter);
			co_await !bzd::print(context_.config.out, "Sending {:d}\n"_csv, counter);
			co_await !context_.config.steadyClock.delay(100_ms);
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
			const auto result = co_await !bzd::async::any(context_.io.receive.get(), context_.config.steadyClock.timeout(2_s));
			co_await !bzd::print(context_.config.out, "Received {:}\n"_csv, result.value());
			++counter;
		}
		co_return {};
	}

private:
	Context& context_;
};

} // namespace example
