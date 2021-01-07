#pragma once

#include "bzd/container/optional.h"
#include "bzd/container/result.h"
#include "bzd/core/assert/minimal.h"
#include "bzd/platform/atomic.h"
#include "bzd/platform/types.h"
#include "bzd/test/inject_point.h"

#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <thread>

namespace bzd::impl {
class SingleLinkedListElement
{
public:
	constexpr SingleLinkedListElement() = default;
	constexpr SingleLinkedListElement(SingleLinkedListElement&& elt) : next_{elt.next_.load()} {} //, previous_{elt.previous_.load()} {}

	bzd::Atomic<SingleLinkedListElement*> next_{nullptr};
	bzd::Atomic<void*> parent_{nullptr};
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

struct ListInjectPoint1
{
};
struct ListInjectPoint2
{
};
struct ListInjectPoint3
{
};
struct ListInjectPoint4
{
};
struct ListInjectPoint5
{
};

/**
 * Implementation of a non-owning doubly linked list.
 * This implementation is thread safe.
 */
template <class T>
class SingleLinkedList
{
public:
	using BasePtrType = SingleLinkedListElement*;
	using PtrType = T*;
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
		BasePtrType node;
		/**
		 * Raw pointer of the next pointer of the previous node.
		 * Raw means without removing deletion mark.
		 */
		BasePtrType nextRaw;
	};

	/**
	 * Look for the previous node of the one passed into argument.
	 * In case of concurrent insertion, the previous node might not be the
	 * actual one, therefore we need to go down the chain to find the actual one.
	 *
	 * \param node We are looking for the previous node of this node.
	 * \param previous A hint to find the previous node.
	 *
	 * \return A structure containing the previous node and the raw pointer its next node.
	 */
	Optional<NodeFound> findPreviousNode(BasePtrType node) noexcept
	{
		NodeFound result{&front_, nullptr};
		bzd::SizeType temp = 1000;

		while (result.node != &back_)
		{
			result.nextRaw = result.node->next_.load();
			const auto previousNext = removeMarks(result.nextRaw);

			// This is the node we are looking for.
			if (previousNext == node)
			{
				return result;
			}

			// Element got deleted concurrently, need to reiterate
			// to find the previous node. Note front_ is used here
			// instead of 'previous' to ensure that elements inserted
			// in other lists will still be found.
			if (!previousNext)
			{
				result.node = &front_;
				continue;
			}

			result.node = previousNext;
		}

		return bzd::nullopt;
	}

public:
	constexpr SingleLinkedList() noexcept { front_.next_.store(&back_); };

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
	[[nodiscard]] Result<void> insert(PtrType element) noexcept
	{
		while (true)
		{
			// Save the previous and next positions of expected future element pointers
			const auto nodePrevious = &front_;
			const auto nodeNext = removeMarks(nodePrevious->next_.load());
			// std::cout << nodeNext << "  " << addDeletionMark(nodeNext) << std::endl;

			bzd::test::InjectPoint<ListInjectPoint1, Args...>();

			// Prepare the current element to be inserted between nodePrevious and nodeNext
			{
				BasePtrType expected{nullptr};
				if (!element->next_.compareExchange(expected, addInsertionMark(nodeNext)))
				{
					//	std::cout << "1 no!" << std::endl;

					return makeError(ListErrorType::elementAlreadyInserted);
				}
			}

			std::cout << "Inserting " << element << std::endl;

			bzd::test::InjectPoint<ListInjectPoint2, Args...>();

			// From here, element cannot be used by any other concurrent operation,
			// as it has already been flagged as inserted.

			bzd::test::InjectPoint<ListInjectPoint3, Args...>();

			{
				BasePtrType expected{nodeNext};
				if (!nodePrevious->next_.compareExchange(expected, element))
				{
					element->next_.store(nullptr);
					continue;
				}
			}

			bzd::test::InjectPoint<ListInjectPoint4, Args...>();

			// Set the parent
			element->parent_.store(this);

			bzd::test::InjectPoint<ListInjectPoint5, Args...>();

			// Remove busy mark
			{
				BasePtrType expected{addInsertionMark(nodeNext)};
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
	[[nodiscard]] Result<void> remove(PtrType element) noexcept
	{
		// Add deletion mark
		{
			BasePtrType expected{element->next_.load()};
			do
			{
				if (!expected || isDeletionMark(expected) || isInsertionMark(expected))
				{
					return makeError(ListErrorType::elementAlreadyRemoved);
				}
			} while (!element->next_.compareExchange(expected, addDeletionMark(expected)));

			// Make sure it exists
			// const auto previous = findPreviousNode(element);
			// if (!previous)

			if (element->parent_.load() != this)
			{
				// Remove busy mark
				expected = addDeletionMark(expected);
				while (!element->next_.compareExchange(expected, removeDeletionMark(expected)))
				{
				}

				return makeError(ListErrorType::notFound);
			}
		}

		// At this point no other concurrent operation can use this element for removal nor insertion

		// bzd::SizeType counter = 10000;

		std::cout << "Deleting " << element << std::endl;

		while (true)
		{
			/*	std::cout << ".";
				if (--counter == 0)
				{
					std::cout << "Timeout loop remove" << std::endl;
					printNode(element);

					BasePtrType cur = &front_;
					while (cur && cur != &back_)
					{
						printNode(cur);
						cur = removeDeletionMark(cur->next_.load());
					}

					int a = 0;
					int b = 12 / a;
				}*/

			// Save the previous and next positions of current element pointers
			const auto nodeNext = removeDeletionMark(element->next_.load());

			bzd::test::InjectPoint<ListInjectPoint1, Args...>();

			// Look for the previous node.
			const auto previous = findPreviousNode(element);
			if (!previous)
			{
				std::cout << "findPreviousNode race" << std::endl;

				// Remove busy mark
				/*	{
						BasePtrType expected{addDeletionMark(nodeNext)};
						while (!element->next_.compareExchange(expected, removeDeletionMark(expected)))
						{
						}
					}*/

				std::cout << "Timeout loop remove" << std::endl;
				printNode(element);

				BasePtrType cur = &front_;
				while (cur && cur != &back_)
				{
					printNode(cur);
					cur = removeDeletionMark(cur->next_.load());
				}

				int a = 0;
				int b = 12 / a;

				// It must exists, as it was checked in the initial stage.
				return makeError(ListErrorType::unhandledRaceCondition);
			}
			auto nodePrevious = previous->node;

			// TODO
			// Ensure the element was not removed
			// if (removeDeletionMark(element->next_.load()) != nodeNext)
			//{
			//	continue;
			//}

			bzd::test::InjectPoint<ListInjectPoint2, Args...>();

			// Ensure that the pointers are valid
			if (!nodeNext)
			{
				std::cout << "null 3 " << nodeNext << std::endl;
				return makeError(ListErrorType::unhandledRaceCondition);
			}

			bzd::test::InjectPoint<ListInjectPoint3, Args...>();

			if (isDeletionMark(previous->nextRaw))
			{
				std::this_thread::yield();
				// std::cout << "DMARK" << std::endl;
				continue;
			}

			// Try to remove the link
			{
				BasePtrType expected{previous->nextRaw};
				if (!nodePrevious->next_.compareExchange(expected,
														 (isInsertionMark(previous->nextRaw)) ? addInsertionMark(nodeNext) : nodeNext))
				// BasePtrType expected{element};
				// if (!nodePrevious->next_.compareExchange(expected, nodeNext))
				{
					/*		std::cout << "RACE! 2 remove" << std::endl;
							std::cout << nodePrevious << " <- " << element << " -> " << nodeNext << std::endl;
							std::cout << nodePrevious << " -> " << nodePrevious->next_.load() << " | ";
							std::cout << nodeNext->previous_.load() << " <- " << nodeNext << std::endl;
		*/
					// nodeNext has been deleted.
					// Try to revert the previously changed node, if it fails, discard it would have meant
					// that the previous has been altered.
					// expected = nodePrevious;
					// nodeNext->previous_.compareExchange(expected, element);

					continue;
				}
			}

			bzd::test::InjectPoint<ListInjectPoint4, Args...>();

			bzd::test::InjectPoint<ListInjectPoint5, Args...>();

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
			if (isDeletionMark(node))
			{
				std::cout << "Deletion mark" << std::endl;
				return makeError(ListErrorType::sanityCheck);
			}

			if (isInsertionMark(node))
			{
				std::cout << "Deletion mark" << std::endl;
				return makeError(ListErrorType::sanityCheck);
			}

			printNode(node);

			if (!node)
			{
				std::cout << "Node cannot be null" << std::endl;
				return makeError(ListErrorType::sanityCheck);
			}

			// Ensure that the next pointer points to the current node.
			if (previous->next_.load() != node)
			{
				std::cout << "Mistmatch previous->next and node" << std::endl;
				return makeError(ListErrorType::sanityCheck);
			}

			// Check if the end is reached.
			if (node == &back_)
			{
				break;
			}

			// Check the element.
			{
				const bool result = sanityCheckElement(reinterpret_cast<T&>(*node));
				if (!result)
				{
					std::cout << "sanityCheckElement failed" << std::endl;
					return makeError(ListErrorType::sanityCheck);
				}
			}

			++counter;
			previous = node;
			node = node->next_.load();
		}

		// Element count should be equal.
		if (size() != counter)
		{
			std::cout << "Counter (" << counter << ") mistmatch with actual size (" << size() << ")" << std::endl;
			return makeError(ListErrorType::sanityCheck);
		}

		return counter;
	}

public:
	void printNode(BasePtrType node)
	{
		const auto printAddress = [this](const BasePtrType address) {
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

		//	printAddress(node->previous_.load());
		//	std::cout << " <- ";
		printAddress(node);
		if (node)
		{
			std::cout << " -> ";
			printAddress(node->next_.load());
		}
		std::cout << std::dec << std::endl;
	}

private:
	constexpr bool isDeletionMark(BasePtrType node) const noexcept { return (reinterpret_cast<IntPtrType>(node) & 1); }
	constexpr bool isInsertionMark(BasePtrType node) const noexcept { return (reinterpret_cast<IntPtrType>(node) & 2); }

	constexpr BasePtrType addDeletionMark(BasePtrType node) const noexcept
	{
		return reinterpret_cast<BasePtrType>(reinterpret_cast<IntPtrType>(node) | 0x1);
	}

	constexpr BasePtrType addInsertionMark(BasePtrType node) const noexcept
	{
		return reinterpret_cast<BasePtrType>(reinterpret_cast<IntPtrType>(node) | 0x2);
	}

	constexpr BasePtrType removeDeletionMark(BasePtrType node) const noexcept
	{
		return reinterpret_cast<BasePtrType>(reinterpret_cast<IntPtrType>(node) & ~3);
	}

	constexpr BasePtrType removeMarks(BasePtrType node) const noexcept
	{
		return reinterpret_cast<BasePtrType>(reinterpret_cast<IntPtrType>(node) & ~3);
	}

private:
	// Having a front & back elemment will simplify the logic to not have to take care of the edge cases.
	SingleLinkedListElement front_;
	SingleLinkedListElement back_;
	bzd::Atomic<SingleLinkedListElement*> last_{nullptr};
	// Number of elements.
	bzd::Atomic<bzd::SizeType> size_{0};
};
} // namespace bzd::impl
