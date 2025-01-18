#include "apps/jardinier2000/jardinier.hh"

#include <driver/gpio.h>
#include <driver/ledc.h>
#include <esp_err.h>
#include <esp_sleep.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace jardinier {

constexpr gpio_num_t waterPumpPin{GPIO_NUM_3};

bzd::Async<> water(bzd::Timer& timer, bzd::OStream& out, bzd::Size wateringTimeS, bzd::UInt64 wakeUpPeriodS)
{
	bzd::Logger logger{out};

	gpio_reset_pin(waterPumpPin);
	gpio_set_direction(waterPumpPin, GPIO_MODE_OUTPUT);

	// blink
	while (true)
	{
		gpio_set_level(waterPumpPin, 1);
		co_await !timer.delay(bzd::units::Millisecond{500});
		gpio_set_level(waterPumpPin, 0);
		co_await !timer.delay(bzd::units::Millisecond{500});
	}

	co_await !logger.info("Watering for {}s..."_csv, wateringTimeS);
	gpio_reset_pin(waterPumpPin);
	gpio_set_direction(waterPumpPin, GPIO_MODE_OUTPUT);
	gpio_set_level(waterPumpPin, 1);
	co_await !timer.delay(bzd::units::Second{wateringTimeS});
	gpio_set_level(waterPumpPin, 0);

	co_await !logger.info("Going to sleep, will wake up in {}s..."_csv, wakeUpPeriodS);
	esp_sleep_enable_timer_wakeup(/*time us*/ wakeUpPeriodS * 1000000ull);
	esp_deep_sleep_start();

	co_return {};
}

} // namespace jardinier
