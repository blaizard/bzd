#pragma once

#include "cc/bzd/container/optional.hh"
#include "cc/bzd/utility/ignore.hh"
#include "cc/bzd/utility/synchronization/spin_mutex.hh"
#include "cc/bzd/utility/synchronization/sync_lock_guard.hh"

namespace bzd::threadsafe {

class NonOwningQueueElement
{
public:
	NonOwningQueueElement() = default;
	// A copy constructor will simply copy the element without copying the next element,
	// this is to ensure consistency with the queue.
	constexpr NonOwningQueueElement(const NonOwningQueueElement&) noexcept {}
    // NOLINTNEXTLINE(bugprone-unhandled-self-assignment)
	constexpr NonOwningQueueElement& operator=(const NonOwningQueueElement&) noexcept { next_ = nullptr; return *this; }
	NonOwningQueueElement(NonOwningQueueElement&&) = delete;
	NonOwningQueueElement& operator=(NonOwningQueueElement&&) = delete;
    ~NonOwningQueueElement() = default;

	NonOwningQueueElement* next_{nullptr};
};

template <class T>
class NonOwningQueue
{
public:
	using Self = NonOwningQueue<T>;
	using ElementType = T;

public:
	constexpr void push(ElementType& element) noexcept
	{
		element.next_ = nullptr;
		const auto lock = makeSyncLockGuard(mutex_);
		auto previousHead = head_;
		head_ = &element;
		if (previousHead)
		{
			previousHead->next_ = head_;
		}
		else
		{
			tail_ = head_;
		}
	}

	[[nodiscard]] constexpr Optional<ElementType&> pop() noexcept
	{
		const auto lock = makeSyncLockGuard(mutex_);
		auto element = tail_;
		if (!element)
		{
			return bzd::nullopt;
		}
		if (head_ == element)
		{
			head_ = nullptr;
		}
		tail_ = element->next_;

		return *static_cast<ElementType*>(element);
	}

	constexpr void clear() noexcept
	{
		while (pop())
		{
		}
	}

protected:
	NonOwningQueueElement* head_{nullptr};
	NonOwningQueueElement* tail_{nullptr};
	SpinMutex mutex_{};
};

} // namespace bzd::threadsafe
