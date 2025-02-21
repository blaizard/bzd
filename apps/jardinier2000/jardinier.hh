#pragma once

#include "apps/jardinier2000/composition.hh"
#include "cc/bzd.hh"

#include <driver/gpio.h>
#include <driver/ledc.h>
#include <esp_err.h>
#include <esp_sleep.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace jardinier {

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
			co_await !context_.io.pump.set(true);
			co_await !context_.config.timer.delay(bzd::units::Millisecond{500});
			co_await !context_.io.pump.set(false);
			co_await !context_.config.timer.delay(bzd::units::Millisecond{500});
		}

		co_await !logger.info("Watering for {}s..."_csv, context_.config.wateringTimeS);
		gpio_reset_pin(waterPumpPin);
		gpio_set_direction(waterPumpPin, GPIO_MODE_OUTPUT);
		gpio_set_level(waterPumpPin, 1);
		co_await !context_.config.timer.delay(bzd::units::Second{context_.config.wateringTimeS});
		gpio_set_level(waterPumpPin, 0);

		co_await !logger.info("Going to sleep, will wake up in {}s..."_csv, context_.config.wakeUpPeriodS);
		esp_sleep_enable_timer_wakeup(/*time us*/ context_.config.wakeUpPeriodS * 1000000ull);
		esp_deep_sleep_start();

		co_return {};
	}

private:
	gpio_num_t waterPumpPin{GPIO_NUM_3};
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

bzd::Async<> water2(bzd::Timer& timer, bzd::OStream& out, bzd::UInt32 wateringTimeS, bzd::UInt32 wakeUpPeriodS, bzd::IO& io);

bzd::Async<> light(int io, bzd::Timer& timer, bzd::OStream& out);

} // namespace jardinier
