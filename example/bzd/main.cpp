#include "bzd.h"

int main()
{
	// Get the default logger
	auto& log = bzd::Registry<bzd::Log>::get("default");

	// Log a new message
	log.info(CSTR("Message: {}\n"), bzd::Registry<bzd::StringView>::get("message"));

	return 0;
}

/*
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#define BLINK_GPIO GPIO_NUM_2 //CONFIG_BLINK_GPIO

int main()
{
	gpio_pad_select_gpio(BLINK_GPIO);
	gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
	while(1) {
		gpio_set_level(BLINK_GPIO, 0);
		vTaskDelay(100 / portTICK_PERIOD_MS);
		gpio_set_level(BLINK_GPIO, 1);
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}
*/
