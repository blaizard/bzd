#pragma once

#include <stdint.h>

namespace bzd
{
	/**
	 * \brief Memory pointer type.
	 */
	typedef void* PtrType;

	/**
	 * \brief Integer type capable of holding a memory pointer
	 */
	typedef intptr_t IntPtrType;

	/**
	 * \brief Function pointer type.
	 */
	typedef void(*FctPtrType)();

	/**
	 * \brief Unsigned integer that can store the maximum size of a theoretically possible object of any type.
	 */
	typedef unsigned long SizeType;

	/**
	 * \brief Signed integer type with a fixed width of 8-bit.
	 */
	typedef int8_t Int8Type;

	/**
	 * \brief Signed integer type with a fixed width of 16-bit.
	 */
	typedef int16_t Int16Type;

	/**
	 * \brief Signed integer type with a fixed width of 32-bit.
	 */
	typedef int32_t Int32Type;

	/**
	 * \brief Signed integer type with a fixed width of 64-bit.
	 */
	typedef int64_t Int64Type;

	/**
	 * \brief Unsigned integer type with a fixed width of 8-bit.
	 */
	typedef uint8_t UInt8Type;

	/**
	 * \brief Unsigned integer type with a fixed width of 16-bit.
	 */
	typedef uint16_t UInt16Type;

	/**
	 * \brief Unsigned integer type with a fixed width of 32-bit.
	 */
	typedef uint32_t UInt32Type;

	/**
	 * \brief Unsigned integer type with a fixed width of 64-bit.
	 */
	typedef uint64_t UInt64Type;
}
