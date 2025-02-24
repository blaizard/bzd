#pragma once

#include "apps/jardinier2000/composition.hh"
#include "cc/bzd.hh"

#include <esp_sleep.h>

namespace jardinier {

template <class Context>
class Controller
{
public:
	constexpr Controller(Context& context) noexcept : context_{context} {}

	bzd::Async<> run()
	{
		co_await !context_.io.lightState.set(jardinier::LightSate::on);
		co_await !context_.io.waterPumpSate.set(jardinier::WaterPumpState::on);
		co_return {};
	}

private:
	Context& context_;
};

template <class Context>
class WaterController
{
public:
	constexpr WaterController(Context& context) noexcept : context_{context} {}

	bzd::Async<> run()
	{
		bzd::Logger logger{context_.config.out};

		// blink
		while (true)
		{
			co_await !context_.io.led.set(1);
			co_await !context_.config.timer.delay(bzd::units::Millisecond{500});
			co_await !context_.io.led.set(0);
			co_await !context_.config.timer.delay(bzd::units::Millisecond{500});
		}

		co_await !logger.info("Watering for {}s..."_csv, context_.config.wateringTimeS);
		co_await !context_.io.pump.set(1);
		co_await !context_.config.timer.delay(bzd::units::Second{context_.config.wateringTimeS});
		co_await !context_.io.pump.set(0);

		co_await !logger.info("Going to sleep, will wake up in {}s..."_csv, context_.config.wakeUpPeriodS);
		esp_sleep_enable_timer_wakeup(/*time us*/ context_.config.wakeUpPeriodS * 1000000ull);
		esp_deep_sleep_start();

		co_return {};
	}

private:
	Context& context_;
};

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

} // namespace jardinier
