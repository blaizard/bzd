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
	virtual ~OChannel() = default;

public:
	/// Write data to an output channel.
	/// The promise resolves only after all the data is transmitted.
	///
	/// \param[in] data The data to be sent via this output channel.
	virtual bzd::Async<> write(const bzd::Span<const T> data) noexcept = 0;

	/// Write data to an output channel.
	/// The data represented by a spans is a collection of contiguous segments,
	/// this function uses this attribute to send segment by segment.
	///
	/// \param[in] data The data to be sent via this output channel.
	template <Size N>
	bzd::Async<> write(const bzd::Spans<const T, N> data) noexcept
	{
		for (const auto& span : data.array())
		{
			auto result = co_await write(span);
			if (!result)
			{
				co_return bzd::move(result).propagate();
			}
		}
		co_return {};
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
	virtual ~IChannel() = default;

public:
	virtual bzd::Async<bzd::Span<T>> read(const bzd::Span<T> data) noexcept = 0;
};

template <class T>
class IOChannel
	: public IChannel<T>
	, public OChannel<T>
{
};

using OStream = OChannel<Byte>;
using IStream = IChannel<Byte>;
using IOStream = IOChannel<Byte>;
} // namespace bzd
