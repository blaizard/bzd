#include "bzd.h"

#include <unistd.h>

#define I2C_MASTER_NUM I2C_NUM_0
#define I2C_MASTER_TX_BUF_DISABLE 0
#define I2C_MASTER_RX_BUF_DISABLE 0

#define ESP_SLAVE_ADDR 0x40
#define ACK_CHECK_EN 1

#define CONFIG_I2C_MASTER_SCL 22
#define CONFIG_I2C_MASTER_SDA 21
#define CONFIG_I2C_MASTER_FREQUENCY 100000

/*static esp_err_t i2c_master_init(void)
{
	i2c_port_t i2c_master_port = I2C_MASTER_NUM;
	i2c_config_t conf;
	conf.mode = I2C_MODE_MASTER;
	conf.sda_io_num = static_cast<gpio_num_t>(CONFIG_I2C_MASTER_SDA);
	conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
	conf.scl_io_num = static_cast<gpio_num_t>(CONFIG_I2C_MASTER_SCL);
	conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
	conf.master.clk_speed = CONFIG_I2C_MASTER_FREQUENCY;
	i2c_param_config(i2c_master_port, &conf);
	return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}*/

/*
static void i2c_master_write_slave(const bzd::UInt8Type reg, const bzd::UInt8Type value)
{
	auto& i2c = bzd::Registry<bzd::OChannel>::get("i2c");
	bzd::Array<bzd::UInt8Type, 3> data{(ESP_SLAVE_ADDR << 1), reg, value};
	i2c.write(data);
}
*/

static void i2c_master_write_slave(const bzd::UInt8Type reg, const bzd::UInt8Type value)
{
	auto& i2c = bzd::Registry<bzd::OChannel>::get("i2c");
	bzd::Array<bzd::UInt8Type, 3> data{(ESP_SLAVE_ADDR << 1) | /*write bit*/ 0, reg, value};
	i2c.write(data.asBytes());
}

#define PCA9685_MODE1 0x00
#define PCA9685_MODE2 0x01
#define PCA9685_PRESCALE 0xFE
#define PCA9685_PRESCALE_MIN 3	 /**< minimum prescale value */
#define PCA9685_PRESCALE_MAX 255 /**< maximum prescale value */
#define FREQUENCY_OSCILLATOR 25000000
#define MODE1_RESTART 0x80 /**< Restart enabled */
#define MODE1_SLEEP 0x10   /**< Low power mode. Oscillator off */
#define MODE1_AI 0x20	   /**< Auto-Increment enabled */
#define PCA9685_LED0_ON_L 0x06

void setPWM(uint8_t num, uint16_t on, uint16_t off)
{
	auto& i2c = bzd::Registry<bzd::OChannel>::get("i2c");
	bzd::Array<bzd::UInt8Type, 6> data{(ESP_SLAVE_ADDR << 1) | /*write bit*/ 0, PCA9685_LED0_ON_L + 4 * num, on, on >> 8, off, off >> 8};
	i2c.write(data.asBytes());
}

#define PCA9685_MODE1_ALLCALL 1
#define PCA9685_MODE1_SUB3 0b10
#define PCA9685_MODE1_SUB2 0b100
#define PCA9685_MODE1_SUB1 0b1000
#define PCA9685_MODE1_SLEEP 0b10000
#define PCA9685_MODE1_AI 0b100000
#define PCA9685_MODE1_EXTCLK 0b1000000
#define PCA9685_MODE1_RESTART 0b10000000

#define PCA9685_MODE2_OUTDRV 0b100
#define PCA9685_MODE2_OCH 0b1000
#define PCA9685_MODE2_INVRT 0b10000

#define PCA9685_OSCILLATOR 25000000

#define PCA9685_PRESCALE_MIN 3
#define PCA9685_PRESCALE_MAX 255

#define PCA9685_UPDATE_RATE_HZ 50

void startSequence2()
{
	i2c_master_write_slave(PCA9685_MODE1,
						   PCA9685_MODE1_SLEEP | PCA9685_MODE1_AI); // Setting mode to sleep so we can change the default PWM frequency

	constexpr bzd::SizeType updateRateHz = PCA9685_UPDATE_RATE_HZ;
	auto prescale = PCA9685_OSCILLATOR / (4096 * updateRateHz) - 1;
	if (prescale < PCA9685_PRESCALE_MIN)
	{
		prescale = PCA9685_PRESCALE_MIN;
	}
	else if (prescale > PCA9685_PRESCALE_MAX)
	{
		prescale = PCA9685_PRESCALE_MAX;
	}
	i2c_master_write_slave(PCA9685_PRESCALE, prescale); // osc_clk/(4096*update_rate) - 1

	i2c_master_write_slave(PCA9685_MODE1, PCA9685_MODE1_AI | PCA9685_MODE1_RESTART); // Set to our preferred mode
	i2c_master_write_slave(PCA9685_MODE2,
						   PCA9685_MODE2_OUTDRV); // Set to our preferred mode[Output logic state not inverted, Outputs change on STOP,
												  // totem pole structure, When OE = 1 (output drivers not enabled), LEDn = 0]
}

#define USMIN 600  // This is the rounded 'minimum' microsecond length based on the minimum pulse of 150
#define USMAX 2400 // This is the rounded 'maximum' microsecond length based on the maximum pulse of 600

void setUs(uint8_t num, bzd::SizeType timeUs)
{
	setPWM(num, 0, timeUs * 4096 * PCA9685_UPDATE_RATE_HZ / 1000000);
}

int main()
{
	// Connect the various channels
	bzd::assert::isResult(bzd::Registry<bzd::OChannel>::get("led").connect());
	bzd::assert::isResult(bzd::Registry<bzd::OChannel>::get("i2c").connect());

	// Get the default logger
	auto& log = bzd::Registry<bzd::Log>::get("default");

	// Log a new message
	log.info(CSTR("Message: {}\n"), bzd::Registry<bzd::StringView>::get("message"));

	// i2c_master_init();

	// LED blinking
	bzd::Array<const char, 1> on{1};
	bzd::Array<const char, 1> off{0};
	auto& port = bzd::Registry<bzd::OChannel>::get("led");
	port.write(on.asBytes());
	sleep(1);
	// vTaskDelay(500 / portTICK_PERIOD_MS);
	port.write(off.asBytes());

	startSequence2();

	// Set all the servos in the middle position

	for (int i = 0; i < 16; ++i) setUs(i, (USMIN + USMAX) / 2);
	/*
		while (true)
		{
			// timeMs
			setUs(0, USMIN);
			sleep(2);
			setUs(0, (USMIN + USMAX) / 2);
			sleep(2);
			setUs(0, USMAX);
			sleep(2);
		}
	*/
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
