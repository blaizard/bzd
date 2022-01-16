#pragma once

#include "cc/bzd/platform/types.hh"

namespace bzd {
template <class T>
struct NumericLimits;

template <>
struct NumericLimits<UInt8Type>
{
	[[nodiscard]] static constexpr UInt8Type min() noexcept { return 0; }
	[[nodiscard]] static constexpr UInt8Type max() noexcept { return 0xff; }
};

template <>
struct NumericLimits<UInt16Type>
{
	[[nodiscard]] static constexpr UInt16Type min() noexcept { return 0; }
	[[nodiscard]] static constexpr UInt16Type max() noexcept { return 0xffff; }
};

template <>
struct NumericLimits<UInt32Type>
{
	[[nodiscard]] static constexpr UInt32Type min() noexcept { return 0; }
	[[nodiscard]] static constexpr UInt32Type max() noexcept { return 0xffffffff; }
};

template <>
struct NumericLimits<UInt64Type>
{
	[[nodiscard]] static constexpr UInt64Type min() noexcept { return 0; }
	[[nodiscard]] static constexpr UInt64Type max() noexcept { return 0xffffffffffffffff; }
};

template <>
struct NumericLimits<Int8Type>
{
	[[nodiscard]] static constexpr Int8Type min() noexcept { return -0x80; }
	[[nodiscard]] static constexpr Int8Type max() noexcept { return 0x7f; }
};

template <>
struct NumericLimits<Int16Type>
{
	[[nodiscard]] static constexpr Int16Type min() noexcept { return -0x8000; }
	[[nodiscard]] static constexpr Int16Type max() noexcept { return 0x7fff; }
};

template <>
struct NumericLimits<Int32Type>
{
	[[nodiscard]] static constexpr Int32Type min() noexcept { return -2147483648; }
	[[nodiscard]] static constexpr Int32Type max() noexcept { return 0x7fffffff; }
};

template <>
struct NumericLimits<Int64Type>
{
	[[nodiscard]] static constexpr Int64Type min() noexcept { return (-9223372036854775807ll - 1ll); }
	[[nodiscard]] static constexpr Int64Type max() noexcept { return 9223372036854775807ll; }
};
} // namespace bzd
