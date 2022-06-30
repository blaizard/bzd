#pragma once

#include "cc/bzd/algorithm/copy_n.hh"
#include "cc/bzd/algorithm/reverse.hh"
#include "cc/bzd/container/span.hh"
#include "cc/bzd/platform/types.hh"

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

template <bzd::Size SizeBytes>
struct SignalInternals
{
	static_assert(SizeBytes > 0 && SizeBytes <= 8, "Signal size must be within 1 and 8 bytes.");
};

template <>
struct SignalInternals<1>
{
	using Type = UInt8;
	using ExtractedType = UInt16;
};

template <>
struct SignalInternals<2>
{
	using Type = UInt16;
	using ExtractedType = UInt32;
};

template <>
struct SignalInternals<3>
{
	using Type = UInt32;
	using ExtractedType = UInt32;
};

template <>
struct SignalInternals<4>
{
	using Type = UInt32;
	using ExtractedType = UInt64;
};

template <>
struct SignalInternals<5>
{
	using Type = UInt64;
	using ExtractedType = UInt64;
};

template <>
struct SignalInternals<6>
{
	using Type = UInt64;
	using ExtractedType = UInt64;
};

template <>
struct SignalInternals<7>
{
	using Type = UInt64;
	using ExtractedType = UInt64;
};
} // namespace bzd::impl

namespace bzd {
template <class Type, bzd::Size OffsetBits, bzd::Size SizeBits, class CompuMethod = impl::ComputeMethodIdentical>
class Signal
{
private:
	static constexpr bzd::Size startByte_ = OffsetBits / 8;
	static constexpr bzd::Size sizeByte_ = ((SizeBits - 1) >> 3) + 1;
	static constexpr bzd::Size shiftBits_ = OffsetBits % 8;
	using SignalInternals = impl::SignalInternals<sizeByte_>;
	static constexpr typename SignalInternals::ExtractedType mask_ = 0xffffffffffffffff >> (64 - SizeBits);

	static_assert(SizeBits > 0, "SizeBits must be greater than 0.");
	static_assert(sizeof(Type) >= sizeByte_, "Type container is smaller than the requested signal size.");

	constexpr void adjustEndianess(typename SignalInternals::Type& value)
	{
		bzd::algorithm::reverse(reinterpret_cast<bzd::Byte*>(&value),
								reinterpret_cast<bzd::Byte*>(&value) + sizeof(typename SignalInternals::Type));
	}

public:
	// Need to handle different endianess
	static constexpr const Type get(const bzd::Span<const bzd::Byte> data)
	{
		typename SignalInternals::ExtractedType extracted;
		bzd::algorithm::copyN(&data.at(startByte_), sizeof(extracted), reinterpret_cast<bzd::Byte*>(&extracted));
		// std::copy_n(&data.at(startByte_), sizeof(extracted), reinterpret_cast<bzd::Byte*>(&extracted));

		const typename SignalInternals::Type type = (extracted >> shiftBits_) & mask_;
		return CompuMethod::template fromBuffer<Type, decltype(type)>(type);
	}

	// This is failing with bazel run //cc/bzd/utility/tests:signal --config=linux_x86_64_gcc --config=prod
	static constexpr void set(const bzd::Span<bzd::Byte> data, const Type& value)
	{
		typename SignalInternals::ExtractedType extracted;

		bzd::algorithm::copyN(&data.at(startByte_), sizeof(extracted), reinterpret_cast<bzd::Byte*>(&extracted));
		// std::copy_n(&data.at(startByte_), sizeof(extracted), reinterpret_cast<bzd::Byte*>(&extracted));

		extracted &= ~(mask_ << shiftBits_);

		const typename SignalInternals::Type valueTyped = CompuMethod::template toBuffer<Type, decltype(valueTyped)>(value);
		extracted |= ((static_cast<decltype(extracted)>(valueTyped) & mask_) << shiftBits_);

		bzd::algorithm::copyN(reinterpret_cast<bzd::Byte*>(&extracted), sizeof(extracted), &data.at(startByte_));
		// std::copy_n(reinterpret_cast<bzd::Byte*>(&extracted), sizeof(extracted), &data.at(startByte_));
	}
};

} // namespace bzd
