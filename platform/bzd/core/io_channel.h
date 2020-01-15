#pragma once

#include "bzd/container/span.h"
#include "bzd/types.h"

namespace bzd {
namespace impl {
template <class T>
class OChannel
{
protected:
	OChannel() = default;

public:
	virtual SizeType write(const Span<const T>& data) noexcept = 0;
};

template <class T>
class IChannel
{
protected:
	IChannel() = default;

public:
	virtual SizeType read(Span<T>& data) noexcept = 0;
};

template <class T>
class IOChannel
	: public IChannel<T>
	, public OChannel<T>
{
};
} // namespace impl

using OChannel = impl::OChannel<bzd::UInt8Type>;
using IChannel = impl::IChannel<bzd::UInt8Type>;
using IOChannel = impl::IOChannel<bzd::UInt8Type>;
} // namespace bzd
