#pragma once

namespace bzd {
enum class Endian
{
	little = __ORDER_LITTLE_ENDIAN__,
	big = __ORDER_BIG_ENDIAN__,
	native = __BYTE_ORDER__
};
}
