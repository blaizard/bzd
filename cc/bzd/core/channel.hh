#pragma once

#include "cc/bzd/container/span.hh"
#include "cc/bzd/core/async.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/core/mutex.hh"

namespace bzd {

template <class T>
class OChannel
{
public:
	/*
	 * Write data to an output channel.
	 * The promise resolves only after all the data is transmitted.
	 * \param data The data to be sent via this output channel.
	 */
	virtual bzd::Async<SizeType> write(const bzd::Span<const T> data) noexcept = 0;

public:
	bzd::Mutex mutex_{};
};

template <class T>
class IChannel
{
public:
	virtual bzd::Async<SizeType> read(const bzd::Span<T> data) noexcept = 0;
};

template <class T>
class IOChannel
	: public IChannel<T>
	, public OChannel<T>
{
};

using OStream = OChannel<ByteType>;
using IStream = IChannel<ByteType>;
using IOStream = IOChannel<ByteType>;
} // namespace bzd
