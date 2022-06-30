#include "apps/jardinier2000/jardinier.hh"

#include <driver/gpio.h>
#include <esp_sleep.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace jardinier {

constexpr gpio_num_t waterPumpPin{GPIO_NUM_23};

bzd::Async<bool> water(bzd::Size wateringTimeS, bzd::UInt64 wakeUpPeriodS)
{
	co_await bzd::log::info("Watering for {}s..."_csv, wateringTimeS);
	gpio_reset_pin(waterPumpPin);
	gpio_set_direction(waterPumpPin, GPIO_MODE_OUTPUT);
	gpio_set_level(waterPumpPin, 1);
	vTaskDelay(configTICK_RATE_HZ * wateringTimeS);
	gpio_set_level(waterPumpPin, 0);

	co_await bzd::log::info("Going to sleep, will wake up in {}s..."_csv, wakeUpPeriodS);
	esp_sleep_enable_timer_wakeup(/*time us*/ wakeUpPeriodS * 1000000);
	esp_deep_sleep_start();

	co_return true;
}

} // namespace jardinier
