#pragma once

#include "bzd/container/result.h"
#include "bzd/container/span.h"
#include "bzd/platform/types.h"

namespace bzd::impl {

class IOChannelCommon
{
public:
	virtual bzd::Result<> connect() { return bzd::nullresult; }
	virtual bzd::Result<> disconnect() { return bzd::nullresult; }
};

class OChannel : public IOChannelCommon
{
protected:
	OChannel() = default;

public:
	virtual bzd::Result<SizeType> write(const Span<const bzd::ByteType>& data) noexcept = 0;
};

class IChannel : public IOChannelCommon
{
protected:
	IChannel() = default;

public:
	virtual bzd::Result<SizeType> read(const Span<bzd::ByteType>& data) noexcept = 0;
};

class IOChannel
	: public IChannel
	, public OChannel
{
};
} // namespace bzd::impl

namespace bzd {
using OChannel = impl::OChannel;
using IChannel = impl::IChannel;
using IOChannel = impl::IOChannel;

} // namespace bzd
