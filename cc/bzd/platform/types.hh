#pragma once

#include <cstddef>
#include <cstdint>

namespace bzd {
/// Memory pointer type.
using PtrType = void*;

/// Integer type capable of holding a memory pointer
using IntPtrType = intptr_t;

/// Function pointer type.
typedef void (*FctPtrType)();

/// Unsigned integer that can store the maximum size of a theoretically
/// possible object of any type.
using SizeType = std::size_t;

/// Boolean type.
using BoolType = bool;

/// Signed integer type with a fixed width of 8-bit.
using Int8Type = std::int8_t;

/// Signed integer type with a fixed width of 16-bit.
using Int16Type = std::int16_t;

/// Signed integer type with a fixed width of 32-bit.
using Int32Type = std::int32_t;

/// Signed integer type with a fixed width of 64-bit.
using Int64Type = std::int64_t;

/// Unsigned integer type with a fixed width of 8-bit.
using UInt8Type = std::uint8_t;

/// Unsigned integer type with a fixed width of 16-bit.
using UInt16Type = std::uint16_t;

/// Unsigned integer type with a fixed width of 32-bit.
using UInt32Type = std::uint32_t;

/// Unsigned integer type with a fixed width of 64-bit.
using UInt64Type = std::uint64_t;

/// 32-bit floating point number.
using Float32Type = float;

/// 64-bit floating point number.
using Float64Type = double;

/// Type representing a byte.
enum class ByteType : std::uint8_t
{
};

/// Constant representing non-position value.
static constexpr const SizeType npos = static_cast<SizeType>(-1);

} // namespace bzd
