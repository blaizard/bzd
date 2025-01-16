#pragma once

#include "cc/bzd/algorithm/move.hh"
#include "cc/bzd/container/span.hh"
#include "cc/bzd/container/spans.hh"
#include "cc/bzd/core/async.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/add_const.hh"
#include "cc/bzd/utility/iterators/distance.hh"
#include "cc/bzd/utility/synchronization/lock_guard.hh"
#include "cc/bzd/utility/synchronization/mutex.hh"

#include <iostream>

namespace bzd {

template <class T>
class OChannel
{
public: // Traits.
	using ValueType = T;
	using ValueConstType = bzd::typeTraits::AddConst<T>;

public:
	OChannel() = default;
	OChannel(const OChannel&) = delete;
	OChannel& operator=(const OChannel&) = delete;
	constexpr OChannel(OChannel&&) noexcept {}
	constexpr OChannel& operator=(OChannel&&) noexcept { return *this; }
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
			co_await !write(span);
		}
		co_return {};
	}

	/// Get a scope lock guard for writing to this channel.
	///
	/// \return A scoped-lock providing exclusive write access to this channel.
	[[nodiscard]] bzd::Async<bzd::lockGuard::Type<bzd::Mutex>> getLock() noexcept { co_return co_await bzd::makeLockGuard(mutex_); }

private:
	bzd::Mutex mutex_{};
};

template <class T>
class IChannel
{
public: // Traits.
	using ValueType = T;
	using ValueConstType = bzd::typeTraits::AddConst<T>;

	using ChannelRangeIterator =
		iterator::InputOrOutputReference<typename bzd::Span<ValueConstType>::Iterator,
										 iterator::InputOrOutputReferencePolicies<typeTraits::IteratorCategory::input>>;
	using Sentinel = typename bzd::Span<bzd::typeTraits::AddConst<T>>::ConstIterator;
	using ChannelRange = ranges::SubRange<ChannelRangeIterator, Sentinel>;

	class GeneratorIChannel : public bzd::Generator<ChannelRange>
	{
	public:
		using bzd::Generator<ChannelRange>::Generator;

	public:
		/// Read the first items from this reader.
		///
		/// \param count The number of items to read.
		///
		/// \return A generator constraint to a number of items.
		GeneratorIChannel first(Size count) noexcept
		{
			auto it = co_await !this->begin();
			while (it != this->end() && count > 0)
			{
				const Size initialSize = it->size();
				const auto size = bzd::min(initialSize, count);
				co_yield ChannelRange{it->begin(), bzd::prev(it->end(), it->size() - size)};

				const auto readLength = initialSize - it->size();
				count -= readLength;
				if (it->size() == 0u)
				{
					co_await !++it;
				}
			}
		}
	};

public:
	IChannel() = default;
	IChannel(const IChannel&) = delete;
	IChannel& operator=(const IChannel&) = delete;
	IChannel(IChannel&&) = default;
	IChannel& operator=(IChannel&&) = default;
	virtual ~IChannel() = default;

protected:
	/// Create a read generator from an input channel.
	///
	/// Return data as long as there is data on this input channel. Returned spans cannot be empty.
	///
	/// It is also important that the user relies on the returned buffer to read the data,
	/// as the input buffer might not be used to avoid unnecessary copies. This provides an interface
	/// to achieve zero copy in certain situation.
	///
	/// \param[in] data A buffer to contain the data to be read.
	/// \return A generator to read the data.
	virtual bzd::Generator<bzd::Span<ValueConstType>> readerImpl(bzd::Span<T>) noexcept
	{
		co_yield bzd::error::Failure("Specialization not implemented."_csv);
	}

public:
	virtual GeneratorIChannel reader(bzd::Span<T> data) noexcept
	{
		auto generator = readerImpl(data);
		auto it = co_await !generator.begin();
		while (it != generator.end())
		{
			auto dataIt = bzd::begin(*it);
			const auto dataEnd = bzd::end(*it);
			do
			{
				co_yield ChannelRange{ChannelRangeIterator{dataIt}, dataEnd};
			} while (dataIt != dataEnd);

			co_await !++it;
		}
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
