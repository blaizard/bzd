#pragma once

#include "cc/bzd/container/optional.hh"
#include "cc/bzd/platform/atomic.hh"
#include "cc/bzd/test/inject_point.hh"
#include "cc/bzd/utility/ignore.hh"

#include <iostream>

namespace bzd::threadsafe {

class NonOwningQueueElement
{
public:
	NonOwningQueueElement() = default;
	// A copy constructor will simply copy the element without copying the next element,
	// this is to ensure consistency with the queue.
	constexpr NonOwningQueueElement(const NonOwningQueueElement&) noexcept {}
	constexpr NonOwningQueueElement& operator=(const NonOwningQueueElement&) noexcept { return *this; }
	NonOwningQueueElement(NonOwningQueueElement&&) = delete;
	NonOwningQueueElement& operator=(NonOwningQueueElement&&) = delete;

	bzd::Atomic<NonOwningQueueElement*> next_{nullptr};
};

template <class T>
class NonOwningQueue
{
public:
	using Self = NonOwningQueue<T>;
	using ElementType = T;

public:
	/// If first run:
	/// head = nullptr -> head = elt1
	/// tail = nullptr -> tail = elt1
	///
	/// If second run:
	/// head = elt1 -> head = elt2
	/// tail = elt1 -> tail = elt1 -> elt2
	template <class... Args>
	void pushFront(ElementType& element) noexcept
	{
		auto previousHead = head_.exchange(&element); //, MemoryOrder::acquireRelease);
		bzd::test::InjectPoint<bzd::test::InjectPoint0, Args...>();
		if (previousHead)
		{
			previousHead->next_.store(&element);
		}
		// Make sure only the first element from the a fresh head will be moved to the tail.
		else
		{
			tail_.store(&element);
		}
	}

	// Race scenario:
	// Assuming: 1 element
	/// head = elt1 -> nullptr
	/// tail = elt1 -> nullptr

	// start pop 0
	// head = elt1 -> nullptr
	// tail = nullptr

	// start push 0
	// head = elt2     elt1 -> nullptr
	// tail = nullptr

	// continue pop
	// head = elt2
	// tail = nullptr

	// continue push
	// head = elt2
	// tail = nullptr
	template <class... Args>
	[[nodiscard]] constexpr Optional<ElementType&> popBack() noexcept
	{
		// Pop the tail element.
		auto element = tail_.exchange(nullptr); //, MemoryOrder::acquireRelease);
		if (!element)
		{
			return bzd::nullopt;
		}
		bzd::test::InjectPoint<bzd::test::InjectPoint0, Args...>();
		// If the head is the same as the element we just poped, we need to set the head to nullptr.
		// If not we can just ignore the result as it means there are more elements.
		{
			auto expected{element};
			bzd::ignore = head_.compareExchange(expected, nullptr);
		}

		// head = elt2
		// tail = nullptr
		// element = elt1
		bzd::test::InjectPoint<bzd::test::InjectPoint1, Args...>();
		auto next = element->next_.load(); // MemoryOrder::acquire);
		bzd::test::InjectPoint<bzd::test::InjectPoint2, Args...>();
		if (next)
		{
			NonOwningQueueElement* expected{nullptr};
			// TODO: need to handle the case where this would fail.
			tail_.compareExchange(expected, next);
		}

		return *static_cast<ElementType*>(element);
	}

	/*
	void print()
	{
		::std::cout << "----------------------------" << ::std::endl;
		::std::cout << "head";
		auto ptr = head_.load();
		while (ptr)
		{
			::std::cout << " -> " << ptr;
			ptr = ptr->next_.load();
		}
		::std::cout << ::std::endl << "tail";
		ptr = tail_.load();
		while (ptr)
		{
			::std::cout << " -> " << ptr;
			ptr = ptr->next_.load();
		}
		::std::cout << ::std::endl;
	}
	*/

protected:
	bzd::Atomic<NonOwningQueueElement*> head_{nullptr};
	bzd::Atomic<NonOwningQueueElement*> tail_{nullptr};
};

} // namespace bzd::threadsafe
