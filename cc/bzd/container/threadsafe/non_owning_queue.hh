#pragma once

#include "cc/bzd/container/optional.hh"
#include "cc/bzd/platform/atomic.hh"
#include "cc/bzd/test/inject_point.hh"
#include "cc/bzd/utility/ignore.hh"

namespace bzd::threadsafe {

class NonOwningQueueElement
{
public:
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
	void push(ElementType& element) noexcept
	{
		auto previousHead = head_.exchange(&element, MemoryOrder::acquireRelease);
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

	template <class... Args>
	[[nodiscard]] constexpr Optional<ElementType&> pop() noexcept
	{
		// Pop the tail element.
		auto element = tail_.exchange(nullptr, MemoryOrder::acquireRelease);
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

		bzd::test::InjectPoint<bzd::test::InjectPoint1, Args...>();
		auto next = element->next_.load(MemoryOrder::acquire);
		bzd::test::InjectPoint<bzd::test::InjectPoint2, Args...>();
		if (next)
		{
			NonOwningQueueElement* expected{nullptr};
			// TODO: need to handle the case where this would fail.
			tail_.compareExchange(expected, next);
		}

		return *static_cast<ElementType*>(element);
	}

protected:
	bzd::Atomic<NonOwningQueueElement*> head_{nullptr};
	bzd::Atomic<NonOwningQueueElement*> tail_{nullptr};
};

} // namespace bzd::threadsafe
