#pragma once

#include "cc/bzd/container/span.h"
#include "cc/bzd/core/async.h"
#include "cc/bzd/platform/types.h"

namespace bzd {

class OChannel
{
public:
	/*
	 * Write data to an output channel.
	 * The promise resolves only after all the data is transmitted.
	 * \param data The data to be sent via this output channel.
	 */
	virtual bzd::Async<SizeType> write(const bzd::Span<const bzd::ByteType> data) noexcept = 0;
};

class IChannel
{
public:
	virtual bzd::Async<SizeType> read(const bzd::Span<bzd::ByteType> data) noexcept = 0;
};

class IOChannel
	: public IChannel
	, public OChannel
{
};
} // namespace bzd
