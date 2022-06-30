#pragma once

#include <cstddef>
#include <cstdint>

namespace bzd {
/// Memory pointer type.
using Pointer = void*;

/// Integer type capable of holding a memory pointer
using IntPointer = intptr_t;

/// Function pointer type.
typedef void (*FctPointer)();

/// Unsigned integer that can store the maximum size of a theoretically
/// possible object of any type.
using Size = std::size_t;

/// Boolean type.
using Bool = bool;

/// Signed integer type with a fixed width of 8-bit.
using Int8 = std::int8_t;

/// Signed integer type with a fixed width of 16-bit.
using Int16 = std::int16_t;

/// Signed integer type with a fixed width of 32-bit.
using Int32 = std::int32_t;

/// Signed integer type with a fixed width of 64-bit.
using Int64 = std::int64_t;

/// Unsigned integer type with a fixed width of 8-bit.
using UInt8 = std::uint8_t;

/// Unsigned integer type with a fixed width of 16-bit.
using UInt16 = std::uint16_t;

/// Unsigned integer type with a fixed width of 32-bit.
using UInt32 = std::uint32_t;

/// Unsigned integer type with a fixed width of 64-bit.
using UInt64 = std::uint64_t;

/// 32-bit floating point number.
using Float32 = float;

/// 64-bit floating point number.
using Float64 = double;

/// Type representing a byte.
enum class Byte : std::uint8_t
{
};

/// Constant representing non-position value.
static constexpr const Size npos = static_cast<Size>(-1);

} // namespace bzd
