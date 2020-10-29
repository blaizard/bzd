#pragma once

#include "bzd/algorithm/copy_n.h"
#include "bzd/algorithm/reverse.h"
#include "bzd/container/buffer.h"
#include "bzd/types.h"

namespace bzd::impl {

class ComputeMethodIdentical
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
	static_assert(SizeBytes > 0 && SizeBytes <= 8, "Singal size must be within 0 and 8 bytes.");
};

template <>
struct SignalInternals<1>
{
	using Type = UInt8Type;
	using ExtractedType = UInt16Type;
};

template <>
struct SignalInternals<2>
{
	using Type = UInt16Type;
	using ExtractedType = UInt32Type;
};

template <>
struct SignalInternals<3>
{
	using Type = UInt32Type;
	using ExtractedType = UInt32Type;
};

template <>
struct SignalInternals<4>
{
	using Type = UInt32Type;
	using ExtractedType = UInt64Type;
};

template <>
struct SignalInternals<5>
{
	using Type = UInt64Type;
	using ExtractedType = UInt64Type;
};

template <>
struct SignalInternals<6>
{
	using Type = UInt64Type;
	using ExtractedType = UInt64Type;
};

template <>
struct SignalInternals<7>
{
	using Type = UInt64Type;
	using ExtractedType = UInt64Type;
};
} // namespace bzd::impl

namespace bzd {
template <class Type, bzd::SizeType OffsetBits, bzd::SizeType SizeBits, class CompuMethod = impl::ComputeMethodIdentical>
class Signal
{
private:
	static constexpr bzd::SizeType startByte_ = OffsetBits / 8;
	static constexpr bzd::SizeType sizeByte_ = ((SizeBits - 1) >> 3) + 1;
	static constexpr bzd::SizeType shiftBits_ = OffsetBits % 8;

	using SignalInternals = impl::SignalInternals<sizeByte_>;

	static constexpr typename SignalInternals::ExtractedType mask_ = 0xffffffffffffffff >> (64 - SizeBits);

	static_assert(sizeof(Type) >= sizeByte_, "Type container is smaller than the requested signal size.");

	constexpr void adjustEndianess(typename SignalInternals::Type& value)
	{
		bzd::algorithm::reverse(reinterpret_cast<bzd::UInt8Type*>(&value),
								reinterpret_cast<bzd::UInt8Type*>(&value) + sizeof(typename SignalInternals::Type));
	}

public:
	// Need to handle different endianess
	static constexpr const Type get(const bzd::ConstBuffer& data)
	{
		typename SignalInternals::ExtractedType extracted;
		bzd::algorithm::copyN(&data.at(startByte_), sizeof(extracted), reinterpret_cast<bzd::UInt8Type*>(&extracted));
		const typename SignalInternals::Type type = (extracted >> shiftBits_) & mask_;
		return CompuMethod::template fromBuffer<Type, decltype(type)>(type);
	}

	static constexpr void set(bzd::Buffer& data, const Type& value)
	{
		typename SignalInternals::ExtractedType extracted;
		bzd::algorithm::copyN(&data.at(startByte_), sizeof(extracted), reinterpret_cast<bzd::UInt8Type*>(&extracted));
		extracted &= ~(mask_ << shiftBits_);

		const typename SignalInternals::Type valueTyped = CompuMethod::template toBuffer<Type, decltype(valueTyped)>(value);
		extracted |= ((static_cast<decltype(extracted)>(valueTyped) & mask_) << shiftBits_);
		bzd::algorithm::copyN(reinterpret_cast<bzd::UInt8Type*>(&extracted), sizeof(extracted), &data.at(startByte_));
	}
};

} // namespace bzd
