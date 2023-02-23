#pragma once

#include "cc/bzd/container/non_owning_list.hh"
#include "cc/bzd/container/threadsafe/ring_buffer.hh"
#include "cc/bzd/core/async.hh"
#include "cc/bzd/core/io/sink.hh"
#include "cc/bzd/core/io/source.hh"
#include "cc/bzd/meta/string_literal.hh"
#include "cc/bzd/utility/synchronization/spin_mutex.hh"
#include "cc/bzd/utility/synchronization/sync_lock_guard.hh"

namespace bzd::io {

template <class T, Size capacity, meta::StringLiteral identifier>
class Buffer
{
private: // Types.
	using Self = Buffer<T, capacity, identifier>;
	using Ring = bzd::threadsafe::RingBuffer<T, capacity>;
	using Value = typename Ring::ValueType;
	using ValueMutable = typename Ring::ValueMutableType;
	using Index = typename Ring::Index;
	struct Element : public bzd::NonOwningListElement
	{
		Index index{};
		bzd::async::ExecutableSuspended executable{};
	};

public: // API.
	constexpr auto makeSource() noexcept { return bzd::io::Source<Self>{*this}; }
	constexpr auto makeSink() noexcept { return bzd::io::Sink<Self>{*this}; }

	constexpr StringView getName() const noexcept { return StringView{identifier.data(), identifier.size()}; }

private:
	/// Notify that new data has been added to the buffer.
	void notifyNewData() noexcept
	{
		const auto lock = makeSyncLockGuard(mutex_);
		const auto index = ring_.indexWrite();
		suspended_.removeIf([index](Element& element) {
			if (index >= element.index)
			{
				element.executable.schedule();
				return true;
			}
			return false;
		});
	}

	/// Wait for new data to arrive.
	bzd::Async<> waitForData(const Index index) noexcept
	{
		Element element;
		element.index = index;
		auto lock = makeSyncLockGuard(mutex_);
		if (ring_.indexWrite() <= index)
		{
			co_await bzd::async::suspend(
				[&](auto&& executable) {
					element.executable = bzd::move(executable);
					bzd::ignore = suspended_.pushFront(element);
					lock.release();
				},
				[&]() {
					auto lock = makeSyncLockGuard(mutex_);
					bzd::ignore = suspended_.erase(element);
				});
		}
		else
		{
			// Still wait for a cycle to avoid the caller to take
			// all CPU if the producer is faster than the consumer.
			co_await bzd::async::yield();
		}

		co_return {};
	}

private:
	friend class bzd::io::Source<Self>;
	friend class bzd::io::Sink<Self>;

	Ring ring_{};
	SpinMutex mutex_{};
	bzd::NonOwningList<Element> suspended_{};
};

} // namespace bzd::io
