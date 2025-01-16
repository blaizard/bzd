#include "apps/jardinier2000/jardinier.hh"

#include <driver/gpio.h>
#include <driver/ledc.h>
#include <esp_err.h>
#include <esp_sleep.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace jardinier {

constexpr gpio_num_t waterPumpPin{GPIO_NUM_3};

#define LEDC_TIMER LEDC_TIMER_0
#define LEDC_MODE LEDC_LOW_SPEED_MODE
#define LEDC_OUTPUT_IO (12) // Define the output GPIO
#define LEDC_CHANNEL LEDC_CHANNEL_0
#define LEDC_DUTY_RES LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define LEDC_DUTY (4096)				// Set duty to 50%. (2 ** 13) * 50% = 4096
#define LEDC_FREQUENCY (4000)			// Frequency in Hertz. Set frequency at 4 kHz

static void example_ledc_init(void)
{
	// Prepare and then apply the LEDC PWM timer configuration
	ledc_timer_config_t ledc_timer{};
	ledc_timer.speed_mode = LEDC_MODE;
	ledc_timer.duty_resolution = LEDC_DUTY_RES;
	ledc_timer.timer_num = LEDC_TIMER;
	ledc_timer.freq_hz = LEDC_FREQUENCY; // Set output frequency at 4 kHz
	ledc_timer.clk_cfg = LEDC_AUTO_CLK;
	ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

	// Prepare and then apply the LEDC PWM channel configuration
	ledc_channel_config_t ledc_channel{};
	ledc_channel.speed_mode = LEDC_MODE;
	ledc_channel.channel = LEDC_CHANNEL;
	ledc_channel.timer_sel = LEDC_TIMER;
	ledc_channel.intr_type = LEDC_INTR_DISABLE;
	ledc_channel.gpio_num = LEDC_OUTPUT_IO;
	ledc_channel.duty = 0; // Set duty to 0%
	ledc_channel.hpoint = 0;
	ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

	// Set duty to 50%
	ESP_ERROR_CHECK(ledc_set_duty(LEDC_MODE, LEDC_CHANNEL, LEDC_DUTY));
	// Update duty to apply the new value
	ESP_ERROR_CHECK(ledc_update_duty(LEDC_MODE, LEDC_CHANNEL));
}

bzd::Async<> water(bzd::Timer& timer, bzd::OStream& out, bzd::Size wateringTimeS, bzd::UInt64 wakeUpPeriodS)
{
	bzd::Logger logger{out};

	gpio_reset_pin(waterPumpPin);
	gpio_set_direction(waterPumpPin, GPIO_MODE_OUTPUT);

	example_ledc_init();

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
