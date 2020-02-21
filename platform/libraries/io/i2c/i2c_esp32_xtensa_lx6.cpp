#include "libraries/io/i2c/i2c_esp32_xtensa_lx6.h"

namespace bzd { namespace io { namespace impl {

bzd::SizeType I2CEsp32XtensaLx6::write(const bzd::Span<const bzd::UInt8Type>& data) noexcept
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
	for (auto&& d : data)
	{
		i2c_master_write_byte(cmd, d, /*ack check enable*/true);
	}
    i2c_master_stop(cmd);
    /*esp_err_t ret =*/ i2c_master_cmd_begin(static_cast<i2c_port_t>(config_.interface), cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
	return data.size();
}

bzd::SizeType I2CEsp32XtensaLx6::read(bzd::Span<bzd::UInt8Type>& data) noexcept
{
	data[0] = 1;
	return 1;
}

}}} // namespace bzd::io
