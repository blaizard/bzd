#include "bzd.h"

int main()
{
	bzd::format::toString(bzd::getOut(), CSTR("The answer is {}.\n"), 42);

    return 0;
}
/*
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"


#define BLINK_GPIO GPIO_NUM_2 //CONFIG_BLINK_GPIO


    bzd::Pin p(12, bzd::Pin::OUTPUT);

    bzd::IOPort p(12);
    p.write(true);


    gpio_pad_select_gpio(BLINK_GPIO);
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    while(1) {
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }

}
*/