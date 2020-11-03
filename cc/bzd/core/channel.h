#pragma once

#include "bzd/container/result.h"
#include "bzd/container/span.h"
#include "bzd/platform/types.h"

namespace bzd::impl {

class IOChannelCommon
{
public:
	virtual bzd::Result<void> connect() { return {}; }
	virtual bzd::Result<void> disconnect() { return {}; }
};

template <class T>
class OChannel : public IOChannelCommon
{
protected:
	OChannel() = default;

public:
	bzd::Result<SizeType> write(const Span<const char>& data)
	{
		return write(Span<const T>(reinterpret_cast<const T*>(data.data()), data.size()));
	}
	virtual bzd::Result<SizeType> write(const Span<const T>& data) noexcept = 0;
	virtual bzd::Result<SizeType> write(const T& data) noexcept { return write(Span<const T>(&data, 1)); }
};

template <class T>
class IChannel : public IOChannelCommon
{
protected:
	IChannel() = default;

public:
	virtual bzd::Result<SizeType> read(Span<T>& data) noexcept = 0;
	virtual bzd::Result<SizeType> read(T& data) noexcept
	{
		auto temp = Span<T>(&data, 1);
		return read(temp);
	}
};

template <class T>
class IOChannel
	: public IChannel<T>
	, public OChannel<T>
{
};
} // namespace bzd::impl

namespace bzd {
using OChannel = impl::OChannel<bzd::UInt8Type>;
using IChannel = impl::IChannel<bzd::UInt8Type>;
using IOChannel = impl::IOChannel<bzd::UInt8Type>;

} // namespace bzd
