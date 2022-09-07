#pragma once

#include "cc/bzd/algorithm/move.hh"
#include "cc/bzd/container/span.hh"
#include "cc/bzd/container/spans.hh"
#include "cc/bzd/core/async.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/add_const.hh"
#include "cc/bzd/utility/distance.hh"
#include "cc/bzd/utility/synchronization/lock_guard.hh"
#include "cc/bzd/utility/synchronization/mutex.hh"

namespace bzd {

template <class T>
class OChannel
{
public: // Traits.
	using ValueType = T;
	using ValueConstType = bzd::typeTraits::AddConst<T>;

public:
	virtual ~OChannel() = default;

public:
	/// Write data to an output channel.
	/// The promise resolves only after all the data is transmitted.
	///
	/// \param[in] data The data to be sent via this output channel.
	virtual bzd::Async<> write(const bzd::Span<ValueConstType> data) noexcept = 0;

	/// Write data to an output channel.
	/// The data represented by a spans is a collection of contiguous segments,
	/// this function uses this attribute to send segment by segment.
	///
	/// \param[in] data The data to be sent via this output channel.
	template <Size count>
	bzd::Async<> write(const bzd::Spans<ValueConstType, count> data) noexcept
	{
		for (const auto& span : data.spans())
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
	[[nodiscard]] bzd::Async<bzd::lockGuard::Type<bzd::Mutex>> getLock() noexcept
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
public: // Traits.
	using ValueType = T;
	using ValueConstType = bzd::typeTraits::AddConst<T>;

public:
	virtual ~IChannel() = default;

public:
	/// Read data from an input channel.
	///
	/// If there is no data available, this function returns immediatly.
	/// It is also important that the user relies on the returned buffer to read the data,
	/// as the input buffer might not be used to avoid unnecessary copies.
	///
	/// \param[in] data A buffer to contain the data to be read.
	/// \return The data read.
	virtual bzd::Async<bzd::Span<ValueConstType>> read(bzd::Span<T>&& data) noexcept = 0;

	/// Read data from an input channel.
	///
	/// Unlike its counter part, this function ensures that the data is copied
	/// into the input buffer.
	///
	/// \param[in] data A buffer to contain the data to be read.
	/// \return The data read that is guaranteed to be part of the buffer passed into argument, or empty.
	bzd::Async<bzd::Span<T>> readToBuffer(bzd::Span<T>&& data) noexcept
	{
		auto dataCopy = data;
		const auto dataRead = co_await !read(bzd::move(dataCopy));
		if (dataRead.empty())
		{
			co_return bzd::Span<T>{};
		}
		// TODO: should be fixed to support data that is written within.
		if (dataRead.data() == data.data())
		{
			// It is safe to const-cast here as we tested that the buffer is part of the one given into argument.
			co_return bzd::Span<T>{const_cast<T*>(dataRead.data()), dataRead.size()};
		}
		bzd::algorithm::copy(dataRead, data);
		co_return data.first(dataRead.size());
	}
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
