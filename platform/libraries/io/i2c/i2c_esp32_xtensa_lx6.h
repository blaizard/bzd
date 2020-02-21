#pragma once

#include "driver/i2c.h"

#include "libraries/io/i2c/i2c.h"

namespace bzd { namespace io { namespace impl {
class I2CEsp32XtensaLx6 : public bzd::io::I2C
{
public:
	struct Configuration : public bzd::io::I2C::Configuration
	{
		bzd::UInt8Type interface = 0;
	};

	constexpr I2CEsp32XtensaLx6(const Configuration& config) : bzd::io::I2C{}, config_{config}
	{
		i2c_port_t i2c_master_port = static_cast<i2c_port_t>(config.interface);
		i2c_config_t conf{};
		if (config.mode == Configuration::Mode::MASTER)
		{
			conf.mode = I2C_MODE_MASTER;
		}
		else
		{
			conf.mode = I2C_MODE_SLAVE;
		}

		conf.sda_io_num = static_cast<gpio_num_t>(config.sda);
		conf.sda_pullup_en = (config.sdaPullup) ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE;

		conf.scl_io_num = static_cast<gpio_num_t>(config.scl);
		conf.scl_pullup_en = (config.sclPullup) ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE;

		conf.master.clk_speed = config.frequency;
		i2c_param_config(i2c_master_port, &conf);
		i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0);

		// TODO handle error
	}

	bzd::SizeType write(const bzd::Span<const bzd::UInt8Type>& data) noexcept override;
	bzd::SizeType read(bzd::Span<bzd::UInt8Type>& data) noexcept override;

private:
	Configuration config_;
};
}}} // namespace bzd::io
