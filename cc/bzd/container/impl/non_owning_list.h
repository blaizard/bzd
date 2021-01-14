#pragma once

#include "bzd/container/optional.h"
#include "bzd/container/result.h"
#include "bzd/core/assert.h"
#include "bzd/platform/atomic.h"
#include "bzd/platform/types.h"
#include "bzd/test/inject_point.h"

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <thread>
namespace bzd::impl {

class ListElementMultiContainer
{
public:
	bzd::Atomic<void*> parent_{nullptr};
};

template <class... Types>
class ListElement : public Types...
{
private:
	using Self = ListElement<Types...>;

public:
	constexpr ListElement() = default;
	constexpr ListElement(Self&& elt) : next_{elt.next_.load()} {}

	bzd::Atomic<Self*> next_{nullptr};
};

enum class ListErrorType
{
	/// The element is already inserted into the list and cannot be insterted twice.
	elementAlreadyInserted,
	elementAlreadyRemoved,
	unhandledRaceCondition,
	notFound,
	sanityCheck,
};

/**
 * Implementation of a non-owning linked list.
 *
 * This implementation is thread safe and lock free.
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
	using BaseElementType = ListElement<ListElementMultiContainer>;
	using BaseElementPtrType = BaseElementType*;

	template <class V>
	using Result = bzd::Result<V, ListErrorType>;

private:
	/**
	 * Structure to return node information when found.
	 */
	struct NodeFound
	{
		/**
		 * Pointer of the previous node.
		 */
		BaseElementPtrType node;
		/**
		 * Raw pointer of the next pointer of the previous node.
		 * Raw means without removing deletion mark.
		 */
		BaseElementPtrType nextRaw;
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
	Optional<NodeFound> findPreviousNode(BaseElementPtrType node) noexcept
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

public:
	constexpr NonOwningList() noexcept { front_.next_.store(&back_); };

	/**
	 * Get the number of elements currently held by this container.
	 *
	 * \return The number of elments.
	 */
	[[nodiscard]] constexpr SizeType size() const noexcept { return size_.load(); }

	/**
	 * Insert an element into the list from the root element.
	 * The idea is to ensure that the last operation is the one that make the element discoverable,
	 * this ensures that any element is consistent.
	 *
	 * Given the following:
	 * | R | -> | A |
	 * |   | <- |   |
	 *
	 * To insert element B, first set the next and previous pointers of this element:
	 * | R | <---------> | A |
	 * |   | <- | B | -> |   |
	 *
	 * What happen if:
	 * - R is deleted: Cannot happen.
	 * - A is deleted: If so, previous pointer to A will be marked as deleted and the next operation will fail.
	 *
	 * Then update A previous pointer to B, if it fails restart from the begining
	 * | R | ----------> | A |
	 * |   | <- | B | -> |   |
	 * |   |    |   | <- |   |
	 *
	 * What happen if:
	 * - R is deleted: Cannot happen.
	 * - A is deleted: Linkage is up-to-date, no problems.
	 * - Concurrent insertion after R: it will fail when update A previous pointer which is expected to be R but it is B.
	 * - Concurrent insertion after A: Linkage is up-to-date, no problems.
	 *
	 * Finally update next pointer of A, if it fails? TBD
	 * | R | <- | B | -> | A |
	 * |   | -> |   | <- |   |
	 */
	template <class... Args>
	[[nodiscard]] Result<void> insert(ElementPtrType element) noexcept
	{
		while (true)
		{
			// Save the previous and next positions of expected future element pointers
			const auto nodePrevious = &front_;
			const auto nodeNext = removeMarks(nodePrevious->next_.load());

			bzd::test::InjectPoint<bzd::test::InjectPoint0, Args...>();

			// Prepare the current element to be inserted between nodePrevious and nodeNext
			{
				BaseElementPtrType expected{nullptr};
				if (!element->next_.compareExchange(expected, setInsertionMark(nodeNext)))
				{
					return makeError(ListErrorType::elementAlreadyInserted);
				}
			}

			bzd::test::InjectPoint<bzd::test::InjectPoint1, Args...>();

			// From here, element cannot be used by any other concurrent operation,
			// as it has already been flagged as inserted.

			bzd::test::InjectPoint<bzd::test::InjectPoint2, Args...>();

			{
				BaseElementPtrType expected{nodeNext};
				if (!nodePrevious->next_.compareExchange(expected, element))
				{
					expected = setWeakMark(nodeNext);
					if (!nodePrevious->next_.compareExchange(expected, element))
					{
						element->next_.store(nullptr);
						continue;
					}
				}
			}

			bzd::test::InjectPoint<bzd::test::InjectPoint3, Args...>();

			// Set the parent
			element->parent_.store(this);

			bzd::test::InjectPoint<bzd::test::InjectPoint4, Args...>();

			// Remove busy mark
			{
				BaseElementPtrType expected{setInsertionMark(nodeNext)};
				while (!element->next_.compareExchange(expected, removeDeletionMark(expected)))
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
	 * (F) -> A -> B  -> C
	 * (F) -> A -> B -x> C
	 * (F) -> A -> B -x> C
	 */
	template <class... Args>
	[[nodiscard]] Result<void> remove(ElementPtrType element) noexcept
	{
		// Add deletion mark
		{
			BaseElementPtrType expected{element->next_.load()};
			do
			{
				if (!expected || isDeletionMark(expected) || isInsertionMark(expected))
				{
					return makeError(ListErrorType::elementAlreadyRemoved);
				}
			} while (!element->next_.compareExchange(expected, setInsertionMark(expected)));

			// Make sure it is part of the same list
			if (element->parent_.load() != this)
			{
				// Remove busy mark
				expected = setInsertionMark(expected);
				while (!element->next_.compareExchange(expected, removeDeletionMark(expected)))
				{
				}

				return makeError(ListErrorType::notFound);
			}

			bzd::test::InjectPoint<bzd::test::InjectPoint0, Args...>();

			expected = setInsertionMark(expected);
			while (!element->next_.compareExchange(expected, setDeletionMark(expected)))
			{
			}
		}

		// At this point no other concurrent operation can use this element for removal nor insertion
		while (true)
		{
			// Save the previous and next positions of current element pointers
			const auto nodeNext = removeDeletionMark(element->next_.load());

			bzd::test::InjectPoint<bzd::test::InjectPoint1, Args...>();

			// Look for the previous node.
			auto previous = findPreviousNode(element);
			if (!previous)
			{
				// If it doesn't find it anymore, it means that a concurrent removal happen.
				// The previous pointer has already been updated, therefore, this node
				// can be removed safely.

				element->next_.store(nullptr);
				break;
			}

			bzd::test::InjectPoint<bzd::test::InjectPoint2, Args...>();

			// Ensure that the pointers are valid
			bzd::assert::isTrue(nodeNext);

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
						BaseElementPtrType expected{removeMarks(previous->nextRaw)};
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
						if (distanceFromElement || removeMarks(current) != element)
						{
							continue;
						}
					}
				}
			}

			const auto nodePrevious = previous->node;

			bzd::test::InjectPoint<bzd::test::InjectPoint4, Args...>();

			if (isDeletionMark(previous->nextRaw))
			{
				continue;
			}

			bzd::test::InjectPoint<bzd::test::InjectPoint5, Args...>();

			// Try to remove the link
			{
				BaseElementPtrType expected{previous->nextRaw};
				if (!nodePrevious->next_.compareExchange(expected,
														 (isInsertionMark(previous->nextRaw)) ? setInsertionMark(nodeNext) : nodeNext))
				{
					continue;
				}
			}

			bzd::test::InjectPoint<bzd::test::InjectPoint6, Args...>();

			// Check if the next element is supposed to be deleted as well,
			// If so tries to

			// Element is completly out of the chain
			// Note here the order is important, the next pointer should be updated
			// the last to make it insertable only at the end.
			element->next_.store(nullptr);

			break;
		}

		// Decrease the element count.
		--size_;

		return nullresult;
	}

	[[nodiscard]] constexpr bzd::Optional<T&> front() noexcept
	{
		auto ptr = front_.next_.load();
		if (ptr == &back_)
		{
			return bzd::nullopt;
		}
		return reinterpret_cast<T&>(*ptr);
	}
	[[nodiscard]] constexpr bzd::Optional<const T&> front() const noexcept
	{
		const auto ptr = front_.next_.load();
		if (ptr == &back_)
		{
			return bzd::nullopt;
		}
		return reinterpret_cast<const T&>(*ptr);
	}
	[[nodiscard]] constexpr bzd::Optional<T&> back() noexcept
	{
		const auto previous = findPreviousNode(&back_);
		if (previous->node == &front_)
		{
			return bzd::nullopt;
		}
		return reinterpret_cast<T&>(*previous->node);
	}

	/**
	 * Ensures that all element are properly connected.
	 * \return The number of node evaluated.
	 */
	template <class U>
	[[nodiscard]] Result<bzd::SizeType> sanityCheck(const U sanityCheckElement = [](const auto&) -> bool { return true; })
	{
		bzd::SizeType counter = 0;
		auto previous = &front_;
		auto node = removeDeletionMark(previous->next_.load());

		printNode(previous);

		while (true)
		{
			bzd::assert::isTrue(!isDeletionMark(node), "Deletion mark");
			bzd::assert::isTrue(!isInsertionMark(node), "Insertion mark");
			bzd::assert::isTrue(node, "Null node");

			printNode(node);

			// Check if the end is reached.
			if (node == &back_)
			{
				const auto maybeBack = back();
				if (!maybeBack)
				{
					bzd::assert::isTrue(counter == 0, CSTR("Expected 0 size but got: {}"), counter);
				}
				else
				{
					bzd::assert::isTrue(previous == &(*maybeBack),
										CSTR("Last pointer is not pointing to the last element: {} vs {}"),
										previous,
										&(*maybeBack));
				}

				break;
			}

			// Check the element.
			{
				const bool result = sanityCheckElement(reinterpret_cast<T&>(*node));
				bzd::assert::isTrue(result, "Custom sanityCheckElement failed");
			}

			++counter;
			previous = node;
			node = node->next_.load();
		}

		// Element count should be equal.
		bzd::assert::isTrue(size() == counter, CSTR("Counter ({}) mistmatch with actual size ({})"), counter, size());

		return counter;
	}

public:
	constexpr void printChain(bool onlyIfFails = true, bzd::SizeType maxElements = 100)
	{
		BaseElementPtrType cur = &front_;
		while (--maxElements && cur && cur != &back_)
		{
			if (!onlyIfFails)
			{
				printNode(cur);
			}
			cur = removeDeletionMark(cur->next_.load());
		}
		if (maxElements == 0)
		{
			if (onlyIfFails)
			{
				printChain(/*onlyIfFails*/ false);
			}
			bzd::assert::unreachable();
		}
	}

	void printNode(BaseElementPtrType node)
	{
		const auto printAddress = [this](const BaseElementPtrType address) {
			std::cout << std::setw(16) << std::showbase << std::hex << reinterpret_cast<uint64_t>(address);
			if (address == &front_)
			{
				std::cout << " (F)";
			}
			else if (address == &back_)
			{
				std::cout << " (B)";
			}
			else
			{
				std::cout << "    ";
			}
		};

		printAddress(node);
		if (node)
		{
			std::cout << " -> ";
			printAddress(node->next_.load());
		}
		std::cout << std::dec << std::endl;
	}

private:
	constexpr bool isDeletionMark(BaseElementPtrType node) const noexcept { return (reinterpret_cast<IntPtrType>(node) & 3) == 1; }
	constexpr bool isInsertionMark(BaseElementPtrType node) const noexcept { return (reinterpret_cast<IntPtrType>(node) & 3) == 2; }
	constexpr bool isWeakMark(BaseElementPtrType node) const noexcept { return (reinterpret_cast<IntPtrType>(node) & 3) == 3; }
	constexpr bool isMark(BaseElementPtrType node) const noexcept { return (reinterpret_cast<IntPtrType>(node) & 3); }

	constexpr BaseElementPtrType setDeletionMark(BaseElementPtrType node) const noexcept
	{
		return reinterpret_cast<BaseElementPtrType>(reinterpret_cast<IntPtrType>(removeMarks(node)) | 0x1);
	}

	constexpr BaseElementPtrType setInsertionMark(BaseElementPtrType node) const noexcept
	{
		return reinterpret_cast<BaseElementPtrType>(reinterpret_cast<IntPtrType>(removeMarks(node)) | 0x2);
	}

	constexpr BaseElementPtrType setWeakMark(BaseElementPtrType node) const noexcept
	{
		return reinterpret_cast<BaseElementPtrType>(reinterpret_cast<IntPtrType>(removeMarks(node)) | 0x3);
	}

	constexpr BaseElementPtrType removeDeletionMark(BaseElementPtrType node) const noexcept
	{
		return reinterpret_cast<BaseElementPtrType>(reinterpret_cast<IntPtrType>(node) & ~3);
	}

	constexpr BaseElementPtrType removeMarks(BaseElementPtrType node) const noexcept
	{
		return reinterpret_cast<BaseElementPtrType>(reinterpret_cast<IntPtrType>(node) & ~3);
	}

private:
	// Having a front & back elemment will simplify the logic to not have to take care of the edge cases.
	BaseElementType front_;
	BaseElementType back_;
	// Number of elements.
	bzd::Atomic<bzd::SizeType> size_{0};
};
} // namespace bzd::impl
