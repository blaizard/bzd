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
	 * \brief Signed integer type with a fixed width.
	 * 
	 * \{
	 */
	typedef int8_t Int8Type;
	typedef int16_t Int16Type;
	typedef int32_t Int32Type;
	typedef int64_t Int64Type;
	//! \}

	/**
	 * \brief Unsigned integer type with a fixed width.
	 * 
	 * \{
	 */
	typedef uint8_t UInt8Type;
	typedef uint16_t UInt16Type;
	typedef uint32_t UInt32Type;
	typedef uint64_t UInt64Type;
	//! \}
}
