#pragma once

#include "bzd/container/optional.h"
#include "bzd/container/result.h"
#include "bzd/core/assert.h"
#include "bzd/platform/atomic.h"
#include "bzd/platform/types.h"
#include "bzd/test/inject_point.h"
#include "bzd/type_traits/conditional.h"

namespace {
	enum class ListErrorType
	{
		elementAlreadyInserted,
		elementAlreadyRemoved,
		notFound,
	};
}

namespace bzd::impl {

/**
 * Implementation of a non-owning linked list.
 *
 * Lock-free, multi-producer, multi-consumer.
 * In addition it supports insertion and deletion from preemptive OS
 * within a critical section.
 * Elements can be moved from one list instance to another.
 *
 * \tparam T Element type.
 */
template <class T>
class NonOwningList
{
public:
	using ElementType = T;
	using ElementPtrType = ElementType*;

	template <class V>
	using Result = bzd::Result<V, ListErrorType>;

public:
	constexpr NonOwningList() noexcept { front_.next_.store(&back_); };

	/**
	 * Get the number of elements currently held by this container.
	 *
	 * \return The number of elments.
	 */
	[[nodiscard]] constexpr SizeType size() const noexcept { return size_.load(); }

	/**
	 * Tells if the list is empty.
	 *
	 * \return True if the list is empty, false otherwise.
	 */
	[[nodiscard]] constexpr bool empty() const noexcept { return size_.load() == 0; }

	/**
	 * Insert an element into the list from the root element.
	 * The idea is to ensure that the last operation is the one that make the element discoverable,
	 * this ensures that any element is consistent.
	 *
	 * Given the following:
	 * | R | -> | A |
	 *
	 * To insert element B, first set the next pointer of this element:
	 * | R | --------> | A |
	 * |   |  | B | -> |   |
	 *
	 * Then update R next pointer to B, if it fails restart from the begining.
	 * | R | -> | B | -> | A |
	 *
	 * \param element Element to be inserted.
	 *
	 * \return An error in case of failure, void otherwise.
	 */
	template <class... Args>
	[[nodiscard]] constexpr Result<void> pushFront(ElementType& element) noexcept
	{
		while (true)
		{
			// Save the previous and next positions of expected future element pointers
			const auto nodePrevious = &front_;
			const auto nodeNext = nodePrevious->next_.load();

			// Make sure none of those conditions are satisfied, they are not possible
			// for the front element.
			bzd::assert::isTrue(nodeNext);
			bzd::assert::isTrue(!isInsertionMark(nodeNext));
			bzd::assert::isTrue(!isDeletionMark(nodeNext));

			bzd::test::InjectPoint<bzd::test::InjectPoint0, Args...>();

			// Prepare the current element to be inserted between nodePrevious and nodeNext,
			// update the next pointer.
			{
				ElementPtrType expected{nullptr};
				if (!element.next_.compareExchange(expected, setInsertionMark(nodeNext)))
				{
					return makeError(ListErrorType::elementAlreadyInserted);
				}
			}

			bzd::test::InjectPoint<bzd::test::InjectPoint1, Args...>();

			// From here, element cannot be used by any other concurrent operation,
			// as it has already been flagged as inserted.

			bzd::test::InjectPoint<bzd::test::InjectPoint2, Args...>();

			// Update the next pointer of the previous element. If it fails check
			// if there was a weak mark.
			{
				ElementPtrType expected{nodeNext};
				if (!nodePrevious->next_.compareExchange(expected, &element))
				{
					element.next_.store(nullptr);
					continue;
				}
			}

			bzd::test::InjectPoint<bzd::test::InjectPoint3, Args...>();

			if constexpr (ElementType::isMultiContainer_)
			{
				// Set the parent
				element.parent_.store(this);
			}

			bzd::test::InjectPoint<bzd::test::InjectPoint4, Args...>();

			// Remove the insertion mark
			{
				ElementPtrType expected{setInsertionMark(nodeNext)};
				while (!element.next_.compareExchange(expected, removeMarks(expected)))
				{
				}
			}

			break;
		}

		// Increase the element count.
		++size_;

		return nullresult;
	}

	/**
	 * Remove an element from the queue.
	 *
	 * \param element Element to be inserted.
	 *
	 * \return An error in case of failure, void otherwise.
	 */
	template <class... Args>
	[[nodiscard]] constexpr Result<void> pop(ElementType& element) noexcept
	{
		// Add deletion mark, this must be done in 2 parts.
		// First set a temporary mark and check wether or not this element
		// is part of this list. If not, remove the mark and return, otherwise
		// replace the mark with the deletion mark.
		if constexpr (ElementType::isMultiContainer_)
		{
			ElementPtrType expected{element.next_.load()};
			do
			{
				if (!expected || isDeletionMark(expected) || isInsertionMark(expected))
				{
					return makeError(ListErrorType::elementAlreadyRemoved);
				}
			} while (!element.next_.compareExchange(expected, setInsertionMark(expected)));

			// Make sure it is part of the same list
			if (element.parent_.load() != this)
			{
				// Remove temporary mark
				expected = setInsertionMark(expected);
				while (!element.next_.compareExchange(expected, removeMarks(expected)))
				{
				}

				return makeError(ListErrorType::notFound);
			}

			bzd::test::InjectPoint<bzd::test::InjectPoint0, Args...>();

			expected = setInsertionMark(expected);
			while (!element.next_.compareExchange(expected, setDeletionMark(expected)))
			{
			}
		}
		else
		{
			ElementPtrType expected{element.next_.load()};
			do
			{
				if (!expected || isDeletionMark(expected) || isInsertionMark(expected))
				{
					return makeError(ListErrorType::elementAlreadyRemoved);
				}
			} while (!element.next_.compareExchange(expected, setDeletionMark(expected)));
		}

		// At this point no other concurrent operation can use this element for removal nor insertion

		while (true)
		{
			// Save the next positions of current element pointers
			const auto nodeNext = removeMarks(element.next_.load());
			bzd::assert::isTrue(nodeNext);

			bzd::test::InjectPoint<bzd::test::InjectPoint1, Args...>();

			// Look for the previous node.
			auto previous = findPreviousNode(&element);
			if (!previous)
			{
				// If it doesn't find it anymore, it means that a concurrent removal happen.
				// The previous pointer has already been updated, therefore, this node
				// can be removed safely.

				element.next_.store(nullptr);
				break;
			}

			bzd::test::InjectPoint<bzd::test::InjectPoint2, Args...>();

			// Go to the left most element that is without the deletion mark.
			// This is to handle concurrent deletion for preemptive systems with
			// critical section behavior.
			{
				bzd::SizeType distanceFromElement{1};
				while (isDeletionMark(previous->nextRaw))
				{
					// Look for the node before
					previous = findPreviousNode(previous->node);
					if (!previous)
					{
						break;
					}
					++distanceFromElement;
				}

				bzd::test::InjectPoint<bzd::test::InjectPoint3, Args...>();

				// The previous node has been altered, then reiterate.
				if (!previous)
				{
					continue;
				}

				// The intent here is to skip more than one element.
				// A mark needs to be set, and ensure that the structure
				// of the elements in between remains.
				if (!isDeletionMark(previous->nextRaw) && distanceFromElement > 1)
				{
					// Set the mark
					{
						ElementPtrType expected{removeMarks(previous->nextRaw)};
						if (!previous->node->next_.compareExchange(expected, setWeakMark(expected)))
						{
							continue;
						}
						previous->nextRaw = setWeakMark(previous->nextRaw);
					}

					// Ensure that the chain remain unchanged
					{
						auto current = previous->nextRaw;
						while (--distanceFromElement)
						{
							current = removeMarks(current)->next_.load();
							if (!isDeletionMark(current))
							{
								break;
							}
						}
						// Something changed in the chain, reiterate.
						if (distanceFromElement || removeMarks(current) != &element)
						{
							continue;
						}
					}
				}
			}

			bzd::test::InjectPoint<bzd::test::InjectPoint4, Args...>();

			// Try to remove the link, if fails reiterate.
			{
				ElementPtrType expected{previous->nextRaw};
				if (!previous->node->next_.compareExchange(expected,
														   (isInsertionMark(previous->nextRaw)) ? setInsertionMark(nodeNext) : nodeNext))
				{
					continue;
				}
			}

			bzd::test::InjectPoint<bzd::test::InjectPoint5, Args...>();

			// Element is completly out of the chain
			element.next_.store(nullptr);
			break;
		}

		// Decrease the element count.
		--size_;

		return nullresult;
	}

protected:
	/**
	 * Structure to return node information when found.
	 */
	struct NodeFound
	{
		/**
		 * Pointer of the previous node.
		 */
		ElementPtrType node;
		/**
		 * Raw pointer of the next pointer of the previous node.
		 * Raw means without removing deletion mark.
		 */
		ElementPtrType nextRaw;
	};

	/**
	 * Look for the previous node of the one passed into argument.
	 * In case of concurrent insertion, the previous node might not be the
	 * actual one, therefore we need to go down the chain to find the actual one.
	 *
	 * \param node We are looking for the previous node of this node.
	 *
	 * \return A structure containing the previous node and the raw pointer its next node.
	 */
	[[nodiscard]] constexpr Optional<NodeFound> findPreviousNode(ElementPtrType node) noexcept
	{
		NodeFound result{&front_, nullptr};
		while (result.node != &back_)
		{
			result.nextRaw = result.node->next_.load();
			const auto next = removeMarks(result.nextRaw);

			// This is the node we are looking for.
			if (next == node)
			{
				return result;
			}

			// Element got deleted concurrently, need to reiterate
			// to find the previous node. Note front_ is used here
			// instead of 'previous' to ensure that elements inserted
			// in other lists will still be found.
			if (!next)
			{
				result.node = &front_;
				continue;
			}

			result.node = next;
		}

		return bzd::nullopt;
	}

protected:
	[[nodiscard]] constexpr bool isDeletionMark(ElementPtrType node) const noexcept
	{
		return (reinterpret_cast<IntPtrType>(node) & 3) == 1;
	}
	[[nodiscard]] constexpr bool isInsertionMark(ElementPtrType node) const noexcept
	{
		return (reinterpret_cast<IntPtrType>(node) & 3) == 2;
	}
	[[nodiscard]] constexpr bool isWeakMark(ElementPtrType node) const noexcept { return (reinterpret_cast<IntPtrType>(node) & 3) == 3; }

	[[nodiscard]] constexpr ElementPtrType setDeletionMark(ElementPtrType node) const noexcept
	{
		return reinterpret_cast<ElementPtrType>(reinterpret_cast<IntPtrType>(removeMarks(node)) | 0x1);
	}

	[[nodiscard]] constexpr ElementPtrType setInsertionMark(ElementPtrType node) const noexcept
	{
		return reinterpret_cast<ElementPtrType>(reinterpret_cast<IntPtrType>(removeMarks(node)) | 0x2);
	}

	[[nodiscard]] constexpr ElementPtrType setWeakMark(ElementPtrType node) const noexcept
	{
		return reinterpret_cast<ElementPtrType>(reinterpret_cast<IntPtrType>(removeMarks(node)) | 0x3);
	}

	[[nodiscard]] constexpr ElementPtrType removeMarks(ElementPtrType node) const noexcept
	{
		return reinterpret_cast<ElementPtrType>(reinterpret_cast<IntPtrType>(node) & ~3);
	}

protected:
	// Having a front & back elemment will simplify the logic to not have to take care of the edge cases.
	ElementType front_;
	ElementType back_;
	// Number of elements.
	bzd::Atomic<bzd::SizeType> size_{0};
};

// ---- NonOwningListElement

class NonOwningListElementVoid
{
};

class NonOwningListElementMultiContainer
{
public:
	// Pointer to the current container.
	bzd::Atomic<void*> parent_{nullptr};
};

template <bzd::BoolType MultiContainer>
class NonOwningListElement
	: public bzd::typeTraits::Conditional<MultiContainer, NonOwningListElementMultiContainer, NonOwningListElementVoid>
{
public:
	static const constexpr bzd::BoolType isMultiContainer_{MultiContainer};

private:
	using Self = NonOwningListElement<MultiContainer>;
	using Container = NonOwningList<Self>;
	template <class V>
	using Result = bzd::Result<V, ListErrorType>;

public:
	constexpr NonOwningListElement() = default;
	constexpr NonOwningListElement(Self&& elt) : next_{elt.next_.load()} {}

	/**
	 * Pop the current element from the list. This is available only if
	 * MultiContainer is enabled, as it has the knowledge of the parent container.
	 */
	template <bzd::BoolType T = MultiContainer, bzd::typeTraits::EnableIf<T, void>* = nullptr>
	[[nodiscard]] constexpr Result<void> pop() noexcept
	{
		auto* container = static_cast<Container*>(this->parent_.load());
		if (container)
		{
			return container->pop(*this);
		}
		return bzd::makeError(ListErrorType::elementAlreadyRemoved);
	}

	// Pointer to the next element from the list.
	bzd::Atomic<Self*> next_{nullptr};
};
} // namespace bzd::impl

namespace bzd {

/**
 * Element for the non-owning list.
 *
 * \tparam MultiContainer Whether multicontainer should be supported or not.
 *         This means that an element can pop from one list and push a different one.
 */
template <bzd::BoolType MultiContainer>
using NonOwningListElement = impl::NonOwningListElement<MultiContainer>;

/**
 * Implementation of a non-owning linked list.
 *
 * The implementation is based on a single linked list, which should be efficient
 * enough for insertion and removal for small lists.
 *
 * Lock-free, multi-producer, multi-consumer.
 * In addition it supports lock-free insertion and deletion from critical section,
 * meaning that a call will always return without the need of task preemption.
 * Elements can be moved from one list instance to another.
 *
 * \tparam T Element type.
 */
template <class T>
class NonOwningList : public bzd::impl::NonOwningList<bzd::NonOwningListElement<T::isMultiContainer_>>
{
public:
	using bzd::impl::NonOwningList<bzd::NonOwningListElement<T::isMultiContainer_>>::NonOwningList;

	[[nodiscard]] constexpr bzd::Optional<T&> front() noexcept
	{
		auto ptr = this->front_.next_.load();
		if (ptr == &this->back_)
		{
			return bzd::nullopt;
		}
		return static_cast<T&>(*ptr);
	}

	[[nodiscard]] constexpr bzd::Optional<const T&> front() const noexcept
	{
		const auto ptr = this->front_.next_.load();
		if (ptr == &this->back_)
		{
			return bzd::nullopt;
		}
		return static_cast<const T&>(*ptr);
	}

	[[nodiscard]] constexpr bzd::Optional<T&> back() noexcept
	{
		const auto previous = this->findPreviousNode(&this->back_);
		if (previous->node == &this->front_)
		{
			return bzd::nullopt;
		}
		return static_cast<T&>(*previous->node);
	}

	[[nodiscard]] constexpr bzd::Optional<const T&> back() const noexcept
	{
		const auto previous = this->findPreviousNode(&this->back_);
		if (previous->node == &this->front_)
		{
			return bzd::nullopt;
		}
		return static_cast<const T&>(*previous->node);
	}
};
} // namespace bzd
