#pragma once

#include <cstddef>
#include <cstdint>

namespace bzd {
/**
 * \brief Memory pointer type.
 */
using PtrType = void*;

/**
 * \brief Integer type capable of holding a memory pointer
 */
using IntPtrType = intptr_t;

/**
 * \brief Function pointer type.
 */
typedef void (*FctPtrType)();

/**
 * \brief Unsigned integer that can store the maximum size of a theoretically
 * possible object of any type.
 */
using SizeType = std::size_t;

/**
 * \brief Boolean type.
 */
using BoolType = bool;

/**
 * \brief Signed integer type with a fixed width of 8-bit.
 */
using Int8Type = std::int8_t;

/**
 * \brief Signed integer type with a fixed width of 16-bit.
 */
using Int16Type = std::int16_t;

/**
 * \brief Signed integer type with a fixed width of 32-bit.
 */
using Int32Type = std::int32_t;

/**
 * \brief Signed integer type with a fixed width of 64-bit.
 */
using Int64Type = std::int64_t;

/**
 * \brief Unsigned integer type with a fixed width of 8-bit.
 */
using UInt8Type = std::uint8_t;

/**
 * \brief Unsigned integer type with a fixed width of 16-bit.
 */
using UInt16Type = std::uint16_t;

/**
 * \brief Unsigned integer type with a fixed width of 32-bit.
 */
using UInt32Type = std::uint32_t;

/**
 * \brief Unsigned integer type with a fixed width of 64-bit.
 */
using UInt64Type = std::uint64_t;

/**
 * \brief Byte type.
 */
enum class ByteType : std::uint8_t
{
};
} // namespace bzd
