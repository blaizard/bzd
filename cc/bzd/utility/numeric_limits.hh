#pragma once

#include "cc/bzd/platform/types.hh"

namespace bzd {
template <class T>
struct NumericLimits;

template <>
struct NumericLimits<UInt8>
{
	[[nodiscard]] static constexpr UInt8 min() noexcept { return 0; }
	[[nodiscard]] static constexpr UInt8 max() noexcept { return 0xff; }
};

template <>
struct NumericLimits<UInt16>
{
	[[nodiscard]] static constexpr UInt16 min() noexcept { return 0; }
	[[nodiscard]] static constexpr UInt16 max() noexcept { return 0xffff; }
};

template <>
struct NumericLimits<UInt32>
{
	[[nodiscard]] static constexpr UInt32 min() noexcept { return 0; }
	[[nodiscard]] static constexpr UInt32 max() noexcept { return 0xffffffff; }
};

template <>
struct NumericLimits<UInt64>
{
	[[nodiscard]] static constexpr UInt64 min() noexcept { return 0; }
	[[nodiscard]] static constexpr UInt64 max() noexcept { return 0xffffffffffffffff; }
};

template <>
struct NumericLimits<Int8>
{
	[[nodiscard]] static constexpr Int8 min() noexcept { return -0x80; }
	[[nodiscard]] static constexpr Int8 max() noexcept { return 0x7f; }
};

template <>
struct NumericLimits<Int16>
{
	[[nodiscard]] static constexpr Int16 min() noexcept { return -0x8000; }
	[[nodiscard]] static constexpr Int16 max() noexcept { return 0x7fff; }
};

template <>
struct NumericLimits<Int32>
{
	[[nodiscard]] static constexpr Int32 min() noexcept { return -2147483648; }
	[[nodiscard]] static constexpr Int32 max() noexcept { return 0x7fffffff; }
};

template <>
struct NumericLimits<Int64>
{
	[[nodiscard]] static constexpr Int64 min() noexcept { return (-9223372036854775807ll - 1ll); }
	[[nodiscard]] static constexpr Int64 max() noexcept { return 9223372036854775807ll; }
};
} // namespace bzd
