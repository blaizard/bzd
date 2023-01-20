#pragma once

#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/range/associate_scope.hh"
#include "cc/bzd/container/spans.hh"
#include "cc/bzd/container/storage/fixed.hh"
#include "cc/bzd/core/assert/minimal.hh"
#include "cc/bzd/platform/atomic.hh"
#include "cc/bzd/utility/scope_guard.hh"
#include "cc/bzd/utility/synchronization/spin_shared_mutex.hh"

namespace bzd::threadsafe {

// Single produce, multi consumer
template <class T, Size capacity>
class RingBuffer
{
public: // Traits.
	using Self = RingBuffer<T, capacity>;
	using StorageType = bzd::impl::FixedStorage<T, capacity>;
	using ValueType = typename StorageType::ValueType;
	using ValueMutableType = typename StorageType::ValueMutableType;

public: // Types.
	template <class U>
	class ScopeOptional : public bzd::Optional<U>
	{
	public:
		constexpr ScopeOptional() noexcept : bzd::Optional<U>{bzd::nullopt} {}
		constexpr explicit ScopeOptional(U value) noexcept : bzd::Optional<U>{value} {}
		constexpr ScopeOptional(U value, bzd::SpinSharedMutex& mutex) noexcept : bzd::Optional<U>{value}, mutex_{&mutex}, shared_{true} {}
		constexpr ScopeOptional(U value, bzd::SpinSharedMutex& mutex, bzd::Atomic<bzd::Size>& counter) noexcept :
			bzd::Optional<U>{value}, mutex_{&mutex}, shared_{false}, counter_{&counter}
		{
		}

		ScopeOptional(const Self&) = delete;
		Self& operator=(const Self&) = delete;
		Self& operator=(Self&&) = delete;
		constexpr ScopeOptional(Self&& other) noexcept :
			bzd::Optional<U>{bzd::move(other)}, mutex_{other.mutex_}, shared_{other.shared_}, counter_{other.counter_}
		{
			other.mutex_ = nullptr;
			other.counter_ = nullptr;
		}

		constexpr ~ScopeOptional() noexcept
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

	private:
		bzd::SpinSharedMutex* mutex_{nullptr};
		bzd::Bool shared_{false};
		bzd::Atomic<bzd::Size>* counter_{nullptr};
	};

public:
	RingBuffer() = default;

	RingBuffer(const Self&) = default;
	Self& operator=(const Self&) = default;
	RingBuffer(Self&&) = default;
	Self& operator=(Self&&) = default;
	~RingBuffer() = default;

public: // Size.
	[[nodiscard]] constexpr bzd::Bool empty() const noexcept { return size() == 0; }
	[[nodiscard]] constexpr bzd::Bool full() const noexcept { return size() >= capacity; }
	[[nodiscard]] constexpr bzd::Size size() const noexcept { return write_.load() - read_.load(); }
	//[[nodiscard]] constexpr bzd::Size capacity() const noexcept { return capacity; }

public:
	/// Get the next element as a scoped reference for writing.
	[[nodiscard]] constexpr ScopeOptional<T&> nextForWriting() noexcept
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
		return {storage_.dataMutable()[index], locks_[index], write_};
	}

	/// Get the last element written to the ring as a scoped reference for reading.
	[[nodiscard]] constexpr ScopeOptional<const T&> lastForReading() noexcept
	{
		// Attempt to reserve the last entry.
		auto [begin, end, maybeIndex] = tryAcquireLockForReading([](bzd::Size& begin, bzd::Size& end) {
			if ((end - begin) > 0)
			{
				begin = end - 1u;
				return true;
			}
			return false;
		});

		if (maybeIndex)
		{
			const auto index = maybeIndex.value();
			return {storage_.data()[index], locks_[index]};
		}

		return {};
	}

	/// Get the first element written to the ring (and not overwritten) as a scoped reference for reading.
	[[nodiscard]] constexpr ScopeOptional<const T&> firstForReading() noexcept
	{
		// Attempt to reserve the first entry.
		auto [begin, end, maybeIndex] = tryAcquireLockForReading([](bzd::Size& begin, bzd::Size& end) { return (end - begin) > 0; });

		if (maybeIndex)
		{
			const auto index = maybeIndex.value();
			return {storage_.data()[index], locks_[index]};
		}

		return {};
	}

	/// Get the a scoped range of the next element available for writing.
	/// If \b count is defined, it will return a scope with the specific number of element.
	/// If not or null, all remaining elements will be returned in the range.
	// [[nodiscard]] constexpr auto asSpansForWriting(const bzd::Size count = 0u) noexcept
	// {
	// }

	/// Get the a scoped range of elements available for reading.
	/// If \b count is defined, it will return a scope with the specific number of element,
	/// starting from the \b first or last depending on the parameter value.
	/// If not or null, all remaining elements will be returned in the range.
	[[nodiscard]] constexpr auto asSpansForReading(const bzd::Size count = 0u, const bzd::Bool first = true) noexcept
	{
		// Attempt to reserve the first entry and compute the indexes of interest.
		auto [begin, end, maybeIndex] = tryAcquireLockForReading([count, first](bzd::Size& begin, bzd::Size& end) {
			if (count)
			{
				if ((end - begin) >= count)
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
			return (end - begin) > 0;
		});

		// Attempt succeeded.
		if (maybeIndex)
		{
			// Lock all the rest until write (but excluding it).
			// It must succeed as nothing can be written until then.
			for (auto index = begin + 1u; index < end; ++index)
			{
				const auto lockAcquired = locks_[index % capacity].tryLockShared();
				bzd::assert::isTrue(lockAcquired);
			}
		}

		bzd::ScopeGuard unlockScope{[begin = begin, end = end, this]() {
			for (auto index = begin; index < end; ++index)
			{
				locks_[index % capacity].unlockShared();
			}
		}};

		const auto spans = makeSpans(storage_.data(), begin, end);
		return range::associateScope(bzd::move(spans), bzd::move(unlockScope));
	}

private:
	/// Try to acquire a reading lock of the entry at the specified index.
	constexpr bzd::Optional<bzd::Size> tryAcquireLockForReading(const bzd::Size read) noexcept
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
		const bzd::Size begin{0u};
		const bzd::Size end{0u};
		const bzd::Optional<bzd::Size> maybeIndex{};
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
	static constexpr bzd::Spans<U, 2u> makeSpans(U* data, const bzd::Size begin, const bzd::Size end) noexcept
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
	bzd::Atomic<bzd::Size> write_{0};
	bzd::Atomic<bzd::Size> read_{0};
};

} // namespace bzd::threadsafe
