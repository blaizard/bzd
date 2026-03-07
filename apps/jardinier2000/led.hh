#pragma once

#include "apps/jardinier2000/composition.hh"
#include "cc/bzd.hh"

namespace jardinier {

template <class Context>
class LEDController
{
public:
	constexpr LEDController(Context& context) noexcept : context_{context} {}

	bzd::Async<> run()
	{
		jardinier::LEDState state{jardinier::LEDState::ok};
		while (true)
		{
			const auto maybeResult = context_.io.state.tryGetNew();
			if (maybeResult)
			{
				state = maybeResult.value();
			}

			switch (state)
			{
			case jardinier::LEDState::ok:
				co_await !context_.io.led.set(1);
				co_await !context_.config.timer.delay(bzd::units::Millisecond{500});
				co_await !context_.io.led.set(0);
				co_await !context_.config.timer.delay(bzd::units::Millisecond{500});
				break;
			default:
				co_await bzd::async::yield();
			}
		}
		co_return {};
	}

private:
	Context& context_;
};

} // namespace jardinier
