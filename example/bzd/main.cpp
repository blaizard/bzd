#include "bzd.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/i2c.h"

#define I2C_MASTER_SCL_IO GPIO_NUM_22
#define I2C_MASTER_SDA_IO GPIO_NUM_21
#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_FREQ_HZ 100000
#define I2C_MASTER_TX_BUF_DISABLE 0
#define I2C_MASTER_RX_BUF_DISABLE 0
#define I2C_PULLUP GPIO_PULLUP_ENABLE

#define ESP_SLAVE_ADDR 0x40
#define ACK_CHECK_EN 1

/*
static esp_err_t i2c_master_init()
{
    const auto i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = I2C_PULLUP;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = I2C_PULLUP;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    i2c_param_config(i2c_master_port, &conf);
    auto ret = i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
	return ret;
}
*/
#define CONFIG_I2C_MASTER_SCL 22
#define CONFIG_I2C_MASTER_SDA 21
#define CONFIG_I2C_MASTER_PORT_NUM I2C_NUM_1
#define CONFIG_I2C_MASTER_FREQUENCY 100000

static esp_err_t i2c_master_init(void)
{
    i2c_port_t i2c_master_port = CONFIG_I2C_MASTER_PORT_NUM;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = static_cast<gpio_num_t>(CONFIG_I2C_MASTER_SDA);
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = static_cast<gpio_num_t>(CONFIG_I2C_MASTER_SCL);
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = CONFIG_I2C_MASTER_FREQUENCY;
    i2c_param_config(i2c_master_port, &conf);
    return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

static esp_err_t i2c_master_write_slave()
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (ESP_SLAVE_ADDR << 1) | I2C_MASTER_WRITE, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, 0xaa, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

int main()
{
	// Get the default logger
	auto& log = bzd::Registry<bzd::Log>::get("default");

	// Log a new message
	log.info(CSTR("Message: {}\n"), bzd::Registry<bzd::StringView>::get("message"));

	i2c_master_init();
	//i2c_master_write_slave();

	// LED blinking
	auto& port = bzd::Registry<bzd::OChannel>::get("led");
	port.write(1);
	vTaskDelay(500 / portTICK_PERIOD_MS);
	port.write(0);
	vTaskDelay(500 / portTICK_PERIOD_MS);

	while (true)
	{
		/*port.write(1);
		vTaskDelay(500 / portTICK_PERIOD_MS);
		port.write(0);*/
		vTaskDelay(500 / portTICK_PERIOD_MS);
		const auto ret = i2c_master_write_slave();
		if (ret == ESP_OK)
		{
			port.write(1);
			vTaskDelay(500 / portTICK_PERIOD_MS);
			port.write(0);
			vTaskDelay(500 / portTICK_PERIOD_MS);
		}

		port.write(1);
		vTaskDelay(100 / portTICK_PERIOD_MS);
		port.write(0);
	}

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
