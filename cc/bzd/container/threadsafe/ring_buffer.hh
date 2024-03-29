#pragma once

#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/spans.hh"
#include "cc/bzd/container/storage/fixed.hh"
#include "cc/bzd/core/assert/minimal.hh"
#include "cc/bzd/platform/atomic.hh"
#include "cc/bzd/utility/max.hh"
#include "cc/bzd/utility/scope_guard.hh"
#include "cc/bzd/utility/synchronization/spin_shared_mutex.hh"

namespace bzd::threadsafe {

/// Result for the ring buffer.
template <class T>
class RingBufferResult : public bzd::Optional<T>
{
public: // Traits.
	using Index = bzd::Size;

public: // Constructors/destructor/...
	constexpr RingBufferResult() noexcept : bzd::Optional<T>{bzd::nullopt} {}
	constexpr explicit RingBufferResult(T value, const Index index) noexcept : bzd::Optional<T>{value}, index_{index} {}
	constexpr RingBufferResult(T value, const Index index, bzd::SpinSharedMutex& mutex) noexcept :
		bzd::Optional<T>{value}, index_{index}, mutex_{&mutex}, shared_{true}
	{
	}
	constexpr RingBufferResult(T value, const Index index, bzd::SpinSharedMutex& mutex, bzd::Atomic<Index>& counter) noexcept :
		bzd::Optional<T>{value}, index_{index}, mutex_{&mutex}, shared_{false}, counter_{&counter}
	{
	}

	RingBufferResult(const RingBufferResult&) = delete;
	RingBufferResult& operator=(const RingBufferResult&) = delete;
	RingBufferResult& operator=(RingBufferResult&&) = delete;
	constexpr RingBufferResult(RingBufferResult&& other) noexcept :
		bzd::Optional<T>{static_cast<bzd::Optional<T>&&>(other)}, index_{other.index_}, mutex_{other.mutex_}, shared_{other.shared_},
		counter_{other.counter_}
	{
		other.mutex_ = nullptr;
		other.counter_ = nullptr;
	}

	constexpr ~RingBufferResult() noexcept
	{
		if (mutex_)
		{
			if (shared_)
			{
				mutex_->unlockShared();
			}
			else
			{
				mutex_->unlock();
			}
		}
		if (counter_)
		{
			++(*counter_);
		}
	}

public: // API.
	[[nodiscard]] constexpr Index index() const noexcept { return index_; }

private:
	const Index index_{0u};
	bzd::SpinSharedMutex* mutex_{nullptr};
	bzd::Bool shared_{false};
	bzd::Atomic<Index>* counter_{nullptr};
};

// Single producer, multi consumer ring buffer.
template <class T, Size capacity>
class RingBuffer
{
public: // Traits.
	using Self = RingBuffer<T, capacity>;
	using StorageType = bzd::impl::FixedStorage<T, capacity>;
	using ValueType = typename StorageType::ValueType;
	using ValueMutableType = typename StorageType::ValueMutableType;
	using Index = typename RingBufferResult<T>::Index;

public:
	RingBuffer() = default;

	RingBuffer(const RingBuffer&) = delete;
	RingBuffer& operator=(const RingBuffer&) = delete;
	RingBuffer(RingBuffer&&) = delete;
	RingBuffer& operator=(RingBuffer&&) = delete;
	~RingBuffer() = default;

public: // Size.
	[[nodiscard]] constexpr bzd::Bool empty() const noexcept { return size() == 0; }
	[[nodiscard]] constexpr bzd::Bool full() const noexcept { return size() >= capacity; }
	[[nodiscard]] constexpr bzd::Size size() const noexcept { return write_.load() - read_.load(); }
	//[[nodiscard]] constexpr bzd::Size capacity() const noexcept { return capacity; }

public: // Indexes.
	[[nodiscard]] constexpr Index indexRead() const noexcept { return read_.load(); }
	[[nodiscard]] constexpr Index indexWrite() const noexcept { return write_.load(); }

public: // Access.
	/// Get the next element as a scoped reference for writing.
	[[nodiscard]] constexpr RingBufferResult<T&> nextForWriting() noexcept
	{
		// Reserve a new entry.
		const auto write = write_.load();
		const auto index = write % capacity;
		const auto lockAcquired = locks_[index].tryLock();
		if (!lockAcquired)
		{
			// Someone is reading the entry, need to wait.
			return {};
		}
		// Update the read pointer if it wraps around, because the written entry
		// is now a previous read entry.
		const auto read = read_.load();
		if (write - read >= capacity)
		{
			++read_;
		}
		return {/*value*/ storage_.dataMutable()[index], /*index*/ write, /*mutex*/ locks_[index], /*counter*/ write_};
	}

	/// Get the last element written to the ring as a scoped reference for reading.
	///
	/// \param start Starting from a specific index.
	[[nodiscard]] constexpr RingBufferResult<const T&> lastForReading(const Index start = 0u) noexcept
	{
		// Attempt to reserve the last entry.
		auto [begin, end, maybeIndex] = tryAcquireLockForReading([start](Index& begin, Index& end) {
			if (end > begin)
			{
				begin = end - 1u;
				return (begin >= start);
			}
			return false;
		});

		if (maybeIndex)
		{
			const auto index = maybeIndex.value();
			return {/*value*/ storage_.data()[index], /*index*/ begin, /*mutex*/ locks_[index]};
		}

		return {};
	}

	/// Get the first element written to the ring (and not overwritten) as a scoped reference for reading.
	///
	/// \param start Starting from a specific index.
	[[nodiscard]] constexpr RingBufferResult<const T&> firstForReading(const Index start = 0u) noexcept
	{
		// Attempt to reserve the first entry.
		auto [begin, end, maybeIndex] = tryAcquireLockForReading([start](Index& begin, Index& end) {
			begin = bzd::max(start, begin);
			return end > begin;
		});

		if (maybeIndex)
		{
			const auto index = maybeIndex.value();
			return {/*value*/ storage_.data()[index], /*index*/ begin, /*mutex*/ locks_[index]};
		}

		return {};
	}

	/// Get the a scoped range of the next element available for writing.
	/// If \b count is defined, it will return a scope with the specific number of element.
	/// If not or null, all remaining elements will be returned in the range.
	// [[nodiscard]] constexpr auto asSpansForWriting(const Index count = 0u) noexcept
	// {
	// }

	/// Get the a scoped range of elements available for reading.
	/// If \b count is defined, it will return a scope with the specific number of element,
	/// starting from the \b first or last depending on the parameter value.
	/// If not or null, all remaining elements will be returned in the range.
	///
	/// \param count The number of element to include the output range. If null, all available elements are returned.
	/// \param first If set, the range will start from the first element inserted, otherwise from the latest.
	/// \param start Starting from a specific index, in other word, excluding every entries oldest that this.
	[[nodiscard]] constexpr RingBufferResult<bzd::Spans<const T, 2u>> asSpansForReading(const Index count = 0u,
																						const Index first = true,
																						const Index start = 0u) noexcept
	{
		// Attempt to reserve the first entry and compute the indexes of interest.
		auto [begin, end, maybeIndex] = tryAcquireLockForReading([count, first, start](Index& begin, Index& end) {
			begin = bzd::max(begin, start);
			if (count)
			{
				if (end >= begin + count)
				{
					if (first)
					{
						end = begin + count;
					}
					else
					{
						begin = end - count;
					}
					return true;
				}
				return false;
			}
			return end > begin;
		});

		// Attempt succeeded.
		// Note that it is enough to only acquire the lock of the first element, as the
		// following cannot be written anyway unless the first is unlocked.
		if (maybeIndex)
		{
			const auto index = maybeIndex.value();
			const auto spans = makeSpans(storage_.data(), begin, end);
			return {/*value*/ spans, /*index*/ begin, /*mutex*/ locks_[index]};
		}

		return {};
	}

private:
	/// Try to acquire a reading lock of the entry at the specified index.
	constexpr bzd::Optional<Index> tryAcquireLockForReading(const Index read) noexcept
	{
		const auto index = read % capacity;
		const auto lockAcquired = locks_[index].tryLockShared();
		if (!lockAcquired)
		{
			// Someone is writing to the entry, need to wait.
			return bzd::nullopt;
		}
		// Ensure the entry is still valid.
		if (read_.load() > read)
		{
			// The read pointer was updated in the meantime and this
			// entry is outdated.
			locks_[index].unlock();
			return bzd::nullopt;
		}
		return index;
	}

	struct ResultForReading
	{
		const Index begin{0u};
		const Index end{0u};
		const bzd::Optional<Index> maybeIndex{};
	};

	/// Try to acquire a reading lock of the entry defined by the callable.
	/// The callable also defines if the function should return a failure or not.
	template <class Callable>
	constexpr ResultForReading tryAcquireLockForReading(Callable&& callable) noexcept
	{
		// Attempt to reserve the first entry.
		auto begin = read_.load();
		auto end = write_.load();
		while (callable(begin, end))
		{
			if (const auto maybeIndex = tryAcquireLockForReading(begin); maybeIndex)
			{
				return {begin, end, maybeIndex};
			}
			// If the entry was not taken, it means that a concurrent write happen,
			// then try again.
			begin = read_.load();
			end = write_.load();
		}
		return {};
	}

	/// Create a Spans of 2 as a range starting from \b being to \b end.
	template <class U>
	static constexpr bzd::Spans<U, 2u> makeSpans(U* data, const Index begin, const Index end) noexcept
	{
		if (begin == end)
		{
			return bzd::Spans<U, 2u>{};
		}
		bzd::assert::isTrue(end - begin <= capacity);
		const auto beginIndex = begin % capacity;
		const auto endIndex = end % capacity;
		const auto firstSpan = bzd::Span<U>{&data[beginIndex], (endIndex > beginIndex) ? (endIndex - beginIndex) : (capacity - beginIndex)};
		const auto secondSpan = (endIndex <= beginIndex) ? bzd::Span<U>{data, endIndex} : bzd::Span<U>{};
		return bzd::Spans<U, 2u>{bzd::inPlace, firstSpan, secondSpan};
	}

private: // Variables.
	StorageType storage_{};
	bzd::SpinSharedMutex locks_[capacity];
	bzd::Atomic<Index> write_{0};
	bzd::Atomic<Index> read_{0};
};

} // namespace bzd::threadsafe
