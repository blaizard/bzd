#pragma once

#include "bzd/container/buffer.h"
#include "bzd/utility/memcpy.h"
#include "bzd/types.h"

#include <iostream>

namespace bzd {
namespace impl {

class CompuMethodIdentical
{
public:
	template <class Type, class BufferType>
	static constexpr BufferType toBuffer(const Type& data)
	{
		return static_cast<BufferType>(data);
	}

	template <class Type, class BufferType>
	static constexpr Type fromBuffer(const BufferType& data)
	{
		return static_cast<Type>(data);
	}
};

template <bzd::SizeType SizeBytes>
struct SignalInternals
{
    static_assert(SizeBytes> 0 && SizeBytes <= 8, "Singal size must be within 0 and 8 bytes.");
};

template<>
struct SignalInternals<1>
{
	using Type = UInt8Type;
	using ExtractedType = UInt16Type;
};

template<>
struct SignalInternals<2>
{
	using Type = UInt16Type;
	using ExtractedType = UInt32Type;
};

template<>
struct SignalInternals<3>
{
	using Type = UInt32Type;
	using ExtractedType = UInt32Type;
};

template<>
struct SignalInternals<4>
{
	using Type = UInt32Type;
	using ExtractedType = UInt64Type;
};

template<>
struct SignalInternals<5>
{
	using Type = UInt64Type;
	using ExtractedType = UInt64Type;
};

template<>
struct SignalInternals<6>
{
	using Type = UInt64Type;
	using ExtractedType = UInt64Type;
};

template<>
struct SignalInternals<7>
{
	using Type = UInt64Type;
	using ExtractedType = UInt64Type;
};
}

template <class Type, bzd::SizeType OffsetBits, bzd::SizeType SizeBits, class CompuMethod = impl::CompuMethodIdentical>
class Signal
{
private:
	static constexpr bzd::SizeType startByte_ = OffsetBits / 8;
	static constexpr bzd::SizeType sizeByte_ = ((SizeBits - 1) >> 3) + 1;
	static constexpr bzd::SizeType shiftBits_ = OffsetBits % 8;

	using SignalInternals = impl::SignalInternals<sizeByte_>;

	static constexpr typename SignalInternals::ExtractedType mask_ = 0xffffffffffffffff >> (64 - SizeBits);

	static_assert(sizeof(Type) >= sizeByte_, "Type container is smaller than the requested signal size.");

public:
	// Need to handle different endianess
	static constexpr const Type get(const bzd::ConstBuffer& data)
	{
		typename SignalInternals::ExtractedType extracted;
		bzd::memcpy(&extracted, &data.at(startByte_), sizeof(extracted));
		const typename SignalInternals::Type type = (extracted >> shiftBits_) & mask_;
		return CompuMethod::template fromBuffer<Type, decltype(type)>(type);
	}

	static constexpr void set(bzd::Buffer& /*data*/, const Type& /*value*/)
	{
	}
};

}
