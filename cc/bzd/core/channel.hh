#pragma once

#include "cc/bzd/container/span.hh"
#include "cc/bzd/core/async.hh"
#include "cc/bzd/core/mutex.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/utility/lock_guard.hh"

namespace bzd {

template <class T>
class OChannel
{
public:
	/// Write data to an output channel.
	/// The promise resolves only after all the data is transmitted.
	/// \param data The data to be sent via this output channel.
	virtual bzd::Async<SizeType> write(const bzd::Span<const T> data) noexcept = 0;

	/// Get a scope lock guard for writing to this channel.
	/// \return The scoped-lock for exclusive write access to this channel.
	[[nodiscard]] bzd::typeTraits::InvokeResult<decltype(bzd::makeLockGuard<bzd::Mutex>), bzd::Mutex&> getLock() noexcept
	{
		auto scope = co_await bzd::makeLockGuard(mutex_);
		co_return bzd::move(scope);
	}

private:
	bzd::Mutex mutex_{};
};

template <class T>
class IChannel
{
public:
	virtual bzd::Async<bzd::Span<T>> read(const bzd::Span<T> data) noexcept = 0;
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
