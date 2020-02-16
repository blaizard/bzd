#pragma once

#include "bzd/container/buffer.h"
#include "bzd/types.h"

namespace bzd {
namespace impl {

template <bzd::SizeType StartByte>
class SignalInternals
{
public:
	static constexpr bzd::SizeType startByte_ = StartByte;
};

}

template <class Type, bzd::SizeType Offset, bzd::SizeType Size>
class Signal
{
private:
	using SignalInternals = bzd::impl::SignalInternals<Offset / 8>;

public:
	static constexpr const Type& get(const bzd::Buffer& data)
	{
		return data.at(SignalInternals::startByte_);
        //auto value = (internal_type&)data.at(SignalInternals::startByte_);
        //return (value >> traits_type::internal_masks::extraction_shift) & traits_type::internal_masks::internal_mask;
	}
};

}
