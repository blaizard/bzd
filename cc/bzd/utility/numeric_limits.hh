#pragma once

#include "cc/bzd/platform/types.hh"

#include <limits>

namespace bzd {

template <class T>
struct NumericLimits;

template <>
struct NumericLimits<UInt8>
{
	[[nodiscard]] static constexpr UInt8 min() noexcept { return ::std::numeric_limits<UInt8>::min(); }
	[[nodiscard]] static constexpr UInt8 max() noexcept { return ::std::numeric_limits<UInt8>::max(); }
};

template <>
struct NumericLimits<UInt16>
{
	[[nodiscard]] static constexpr UInt16 min() noexcept { return ::std::numeric_limits<UInt16>::min(); }
	[[nodiscard]] static constexpr UInt16 max() noexcept { return ::std::numeric_limits<UInt16>::max(); }
};

template <>
struct NumericLimits<UInt32>
{
	[[nodiscard]] static constexpr UInt32 min() noexcept { return ::std::numeric_limits<UInt32>::min(); }
	[[nodiscard]] static constexpr UInt32 max() noexcept { return ::std::numeric_limits<UInt32>::max(); }
};

template <>
struct NumericLimits<UInt64>
{
	[[nodiscard]] static constexpr UInt64 min() noexcept { return ::std::numeric_limits<UInt64>::min(); }
	[[nodiscard]] static constexpr UInt64 max() noexcept { return ::std::numeric_limits<UInt64>::max(); }
};

template <>
struct NumericLimits<Int8>
{
	[[nodiscard]] static constexpr Int8 min() noexcept { return ::std::numeric_limits<Int8>::min(); }
	[[nodiscard]] static constexpr Int8 max() noexcept { return ::std::numeric_limits<Int8>::max(); }
};

template <>
struct NumericLimits<Int16>
{
	[[nodiscard]] static constexpr Int16 min() noexcept { return ::std::numeric_limits<Int16>::min(); }
	[[nodiscard]] static constexpr Int16 max() noexcept { return ::std::numeric_limits<Int16>::max(); }
};

template <>
struct NumericLimits<Int32>
{
	[[nodiscard]] static constexpr Int32 min() noexcept { return ::std::numeric_limits<Int32>::min(); }
	[[nodiscard]] static constexpr Int32 max() noexcept { return ::std::numeric_limits<Int32>::max(); }
};

template <>
struct NumericLimits<Int64>
{
	[[nodiscard]] static constexpr Int64 min() noexcept { return ::std::numeric_limits<Int64>::min(); }
	[[nodiscard]] static constexpr Int64 max() noexcept { return ::std::numeric_limits<Int64>::max(); }
};

template <>
struct NumericLimits<Float32>
{
	[[nodiscard]] static constexpr Float32 min() noexcept { return ::std::numeric_limits<Float32>::min(); }
	[[nodiscard]] static constexpr Float32 max() noexcept { return ::std::numeric_limits<Float32>::max(); }
	[[nodiscard]] static constexpr Bool isIEC559() noexcept { return ::std::numeric_limits<Float32>::is_iec559; }
};

template <>
struct NumericLimits<Float64>
{
	[[nodiscard]] static constexpr Float64 min() noexcept { return ::std::numeric_limits<Float64>::min(); }
	[[nodiscard]] static constexpr Float64 max() noexcept { return ::std::numeric_limits<Float64>::max(); }
	[[nodiscard]] static constexpr Bool isIEC559() noexcept { return ::std::numeric_limits<Float64>::is_iec559; }
};

} // namespace bzd
