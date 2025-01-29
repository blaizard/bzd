#pragma once

#include "apps/jardinier2000/composition.hh"
#include "cc/bzd.hh"

namespace jardinier {

template <class Context>
class LightController
{
public:
	constexpr LightController(Context& context) noexcept : context_{context} {}

	bzd::Async<> run()
	{
		bzd::Logger logger{context_.config.out};

		co_await !logger.info("Light controller run."_csv);

		const int max = 4096 * 2;
		int current = 0;
		int inc = 2;
		while (true)
		{
			while (current >= 0 && current <= max)
			{
				co_await !context_.io.pwmDuty0.set(current);
				co_await !context_.io.pwmDuty1.set(current);
				current += inc;
				co_await !context_.config.timer.delay(bzd::units::Millisecond{1});
			}
			if (inc > 0)
			{
				current = max;
				inc = -2;
			}
			else
			{
				current = 0;
				inc = 2;
			}
		}

		co_return {};
	}

private:
	Context& context_;
};

bzd::Async<> water(bzd::Timer& timer, bzd::OStream& out, bzd::UInt32 wateringTimeS, bzd::UInt32 wakeUpPeriodS);

bzd::Async<> light(int io, bzd::Timer& timer, bzd::OStream& out);

} // namespace jardinier
