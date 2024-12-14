#pragma once

#include <bit>

namespace bzd {
template <class To, class From>
constexpr To bitCast(const From& src) noexcept
{
	return std::bit_cast(src);
}
} // namespace bzd
