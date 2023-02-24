#pragma once

#include "cc/bzd/container/optional.hh"
#include "cc/bzd/utility/ignore.hh"
#include "cc/bzd/utility/synchronization/spin_mutex.hh"
#include "cc/bzd/utility/synchronization/sync_lock_guard.hh"

namespace bzd::threadsafe {

class NonOwningQueueSpinElement
{
public:
	NonOwningQueueSpinElement() = default;
	// A copy constructor will simply copy the element without copying the next element,
	// this is to ensure consistency with the queue.
	constexpr NonOwningQueueSpinElement(const NonOwningQueueSpinElement&) noexcept {}
	// NOLINTNEXTLINE(bugprone-unhandled-self-assignment)
	constexpr NonOwningQueueSpinElement& operator=(const NonOwningQueueSpinElement&) noexcept
	{
		next_ = nullptr;
		return *this;
	}
	NonOwningQueueSpinElement(NonOwningQueueSpinElement&&) = delete;
	NonOwningQueueSpinElement& operator=(NonOwningQueueSpinElement&&) = delete;
	~NonOwningQueueSpinElement() = default;

protected:
	[[nodiscard]] constexpr Bool isDetached() const noexcept { return (next_ == nullptr); }

private:
	template <class T>
	friend class NonOwningQueueSpin;

	NonOwningQueueSpinElement* next_{nullptr};
};

template <class T>
class NonOwningQueueSpin
{
public:
	using Self = NonOwningQueueSpin<T>;
	using ElementType = T;

public:
	constexpr void pushFront(ElementType& element) noexcept
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

	[[nodiscard]] constexpr Optional<ElementType&> popBack() noexcept
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
		element->next_ = nullptr;

		return *static_cast<ElementType*>(element);
	}

	constexpr void clear() noexcept
	{
		const auto lock = makeSyncLockGuard(mutex_);
		auto ptr = &tail_;
		while (*ptr)
		{
			auto temp = &((*ptr)->next_);
			*ptr = nullptr;
			ptr = temp;
		}
		head_ = nullptr;
	}

	/*
	void print()
	{
		::std::cout << "----------------------------" << ::std::endl;
		::std::cout << "head";
		auto ptr = head_;
		while (ptr)
		{
			::std::cout << " -> " << ptr;
			ptr = ptr->next_;
		}
		::std::cout << ::std::endl << "tail";
		ptr = tail_;
		while (ptr)
		{
			::std::cout << " -> " << ptr;
			ptr = ptr->next_;
		}
		::std::cout << ::std::endl;
	}
	*/

protected:
	NonOwningQueueSpinElement* head_{nullptr};
	NonOwningQueueSpinElement* tail_{nullptr};
	SpinMutex mutex_{};
};

} // namespace bzd::threadsafe
