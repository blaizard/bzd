#include "cc/libraries/io/i2c/i2c_esp32_xtensa_lx6.hh"

#include "driver/i2c.hh"
namespace bzd::io::impl {

bzd::Result<> I2CEsp32XtensaLx6::connect()
{
	i2c_port_t i2c_master_port = static_cast<i2c_port_t>(config_.interface);
	i2c_config_t conf{};
	if (config_.mode == Configuration::Mode::MASTER)
	{
		conf.mode = I2C_MODE_MASTER;
	}
	else
	{
		conf.mode = I2C_MODE_SLAVE;
	}

	conf.sda_io_num = static_cast<gpio_num_t>(config_.sda);
	conf.sda_pullup_en = (config_.sdaPullup) ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE;

	conf.scl_io_num = static_cast<gpio_num_t>(config_.scl);
	conf.scl_pullup_en = (config_.sclPullup) ? GPIO_PULLUP_ENABLE : GPIO_PULLUP_DISABLE;

	conf.master.clk_speed = config_.frequency;

	i2c_param_config(i2c_master_port, &conf);
	i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0);

	return bzd::nullresult;
}

bzd::Result<Size> I2CEsp32XtensaLx6::write(const bzd::Span<const bzd::UInt8>& data) noexcept
{
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	for (auto&& d : data)
	{
		i2c_master_write_byte(cmd, d, /*ack check enable*/ true);
	}
	i2c_master_stop(cmd);
	/*esp_err_t ret =*/i2c_master_cmd_begin(static_cast<i2c_port_t>(config_.interface), cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	return data.size();
}

bzd::Result<Size> I2CEsp32XtensaLx6::read(bzd::Span<bzd::UInt8>& data) noexcept
{
	data[0] = 1;
	return 1;
}

} // namespace bzd::io::impl
