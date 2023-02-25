#pragma once

#include "cc/bzd/container/optional.hh"
#include "cc/bzd/utility/ignore.hh"
#include "cc/bzd/utility/synchronization/spin_mutex.hh"
#include "cc/bzd/utility/synchronization/sync_lock_guard.hh"

namespace bzd::threadsafe {

class NonOwningRingSpinElement
{
public:
	NonOwningRingSpinElement() = default;
	// A copy constructor will simply copy the element without copying the next element,
	// this is to ensure consistency with the queue.
	constexpr NonOwningRingSpinElement(const NonOwningRingSpinElement&) noexcept {}
	// NOLINTNEXTLINE(bugprone-unhandled-self-assignment)
	constexpr NonOwningRingSpinElement& operator=(const NonOwningRingSpinElement&) noexcept
	{
		next_ = nullptr;
		return *this;
	}
	NonOwningRingSpinElement(NonOwningRingSpinElement&&) = delete;
	NonOwningRingSpinElement& operator=(NonOwningRingSpinElement&&) = delete;
	~NonOwningRingSpinElement() = default;

protected:
	[[nodiscard]] constexpr Bool isDetached() const noexcept { return (next_ == nullptr); }

private:
	template <class T>
	friend class NonOwningRingSpin;

	NonOwningRingSpinElement* next_{nullptr};
};

template <class T>
class NonOwningRingSpin
{
public:
	using Self = NonOwningRingSpin<T>;
	using ElementType = T;

public:
	/// Push an element at the back of the ring, it will be the last to be poped.
	constexpr void pushBack(ElementType& element) noexcept
	{
		const auto lock = makeSyncLockGuard(mutex_);

		element.next_ = head_;
		tail_->next_ = &element;
		tail_ = &element;
	}

	/// Push an element at the front of the ring, it will be the first to be poped.
	constexpr void pushFront(ElementType& element) noexcept
	{
		const auto lock = makeSyncLockGuard(mutex_);

		element.next_ = head_;
		tail_->next_ = &element;
		head_ = &element;
	}

	template <class Callable>
	[[nodiscard]] constexpr Optional<ElementType&> popFront(Callable&& callable) noexcept
	{
		const auto lock = makeSyncLockGuard(mutex_);

		auto originalTail = tail_;
		while (head_ != originalTail)
		{
			// Advance if no match.
			if (head_ == &dummy_ || !callable(*static_cast<ElementType*>(head_)))
			{
				tail_ = head_;
				head_ = head_->next_;
			}
			else
			{
				auto element = head_;
				head_ = element->next_;
				tail_->next_ = head_;
				element->next_ = nullptr;

				return *static_cast<ElementType*>(element);
			}
		}

		return bzd::nullopt;
	}

	[[nodiscard]] constexpr Optional<ElementType&> popFront() noexcept
	{
		return popFront([](ElementType&) -> bool { return true; });
	}

	constexpr void clear() noexcept
	{
		Optional<ElementType&> element{};
		do
		{
			element = popFront();
		} while (element.hasValue());
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
			if (ptr == head_)
			{
				break;
			}
		}
		::std::cout << ::std::endl << "tail";
		ptr = tail_;
		while (ptr)
		{
			::std::cout << " -> " << ptr;
			ptr = ptr->next_;
			if (ptr == tail_)
			{
				break;
			}
		}
		::std::cout << ::std::endl;
	}
	*/

protected:
	NonOwningRingSpinElement dummy_{};
	NonOwningRingSpinElement* head_{&dummy_};
	NonOwningRingSpinElement* tail_{&dummy_};
	SpinMutex mutex_{};
};

} // namespace bzd::threadsafe
