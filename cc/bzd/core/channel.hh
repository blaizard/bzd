#pragma once

#include "cc/bzd/container/span.hh"
#include "cc/bzd/container/spans.hh"
#include "cc/bzd/core/async.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/utility/synchronization/lock_guard.hh"
#include "cc/bzd/utility/synchronization/mutex.hh"

namespace bzd {

template <class T>
class OChannel
{
public:
	/// Write data to an output channel.
	/// The promise resolves only after all the data is transmitted.
	///
	/// \param[in] data The data to be sent via this output channel.
	/// \return The amount of data written.
	virtual bzd::Async<SizeType> write(const bzd::Span<const T> data) noexcept = 0;

	/// Write data to an output channel.
	/// The data represented by a spans is a collection of contiguous segments,
	/// this function uses this attribute to send segment by segment.
	///
	/// \param[in] data The data to be sent via this output channel.
	/// \return The amount of data written.
	template <SizeType N>
	bzd::Async<SizeType> write(const bzd::Spans<const T, N> data) noexcept
	{
		SizeType size = 0;
		for (const auto& span : data.array())
		{
			auto result = co_await write(span);
			if (result)
			{
				size += result.value();
			}
			else
			{
				co_return result.propagate();
			}
		}
		co_return size;
	}

	/// Get a scope lock guard for writing to this channel.
	///
	/// \return A scoped-lock providing exclusive write access to this channel.
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
