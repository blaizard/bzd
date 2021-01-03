#pragma once

#include "bzd/container/optional.h"
#include "bzd/container/result.h"
#include "bzd/core/assert/minimal.h"
#include "bzd/platform/atomic.h"
#include "bzd/platform/types.h"
#include "bzd/test/inject_point.h"

#include <iostream>

namespace bzd::impl {
class CircularListElement
{
public:
	constexpr CircularListElement() = default;
	constexpr CircularListElement(CircularListElement&& elt) : next_{elt.next_.load()}, previous_{elt.previous_.load()} {}

	bzd::Atomic<CircularListElement*> next_{};
	bzd::Atomic<CircularListElement*> previous_{};
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
 * Implementation of a non-owning circular double linked list.
 * This implementation is thread safe.
 */
template <class T>
class CircularList
{
public:
	using BasePtrType = CircularListElement*;
	using PtrType = T*;
	template <class V>
	using Result = bzd::Result<V, ListErrorType>;

private:
	const BasePtrType deleteMark_{reinterpret_cast<const BasePtrType>(1)};

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
	Result<NodeFound> findPreviousNode(BasePtrType node, BasePtrType previous) noexcept
	{
		NodeFound result{previous, nullptr};

		while (result.node)
		{
			result.nextRaw = result.node->next_.load();
			const auto previousNext = removeDeletionMark(result.nextRaw);

			// This is the node we are looking for.
			if (previousNext == node)
			{
				return result;
			}

			// Element got deleted concurrently, need to reiterate
			// to find the previous node. Note head_ is used here
			// instead of 'previous' to ensure that elements inserted
			// in other lists will still be found.
			if (!previousNext)
			{
				result.node = &head_;
				continue;
			}

			// Node was not found within the chain.
			if (previousNext == &tail_)
			{
				return makeError(ListErrorType::notFound);
			}
			result.node = previousNext;
		}

		return makeError(ListErrorType::unhandledRaceCondition);
	}

public:
	constexpr CircularList() noexcept
	{
		head_.next_.store(&tail_);
		tail_.previous_.store(&head_);
	};

	/**
	 * Get the number of elements currently held by this container.
	 * 
	 * \return The number of elments.
	 */
	constexpr SizeType size() const noexcept { return size_.load(); }

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
	Result<void> insert(PtrType element) noexcept
	{
		bzd::SizeType retry = 100000;

		while (true)
		{
			if (--retry == 0)
			{
				std::cout << "ERROR" << std::endl;
				volatile char* tmo = nullptr;
				tmo[13] = 2;
			}

			// Save the previous and next positions of expected future element pointers
			const auto nodePrevious = &head_;
			const auto nodeNext = removeDeletionMark(nodePrevious->next_.load());

			bzd::test::InjectPoint<ListInjectPoint1, Args...>();

			// Prepare the current element to be inserted between nodePrevious and nodeNext
			{
				BasePtrType expected{nullptr};
				if (!element->next_.compareExchange(expected, addDeletionMark(nodeNext)))
				{
					std::cout << "1 no!" << std::endl;

					return makeError(ListErrorType::elementAlreadyInserted);
				}
			}

			bzd::test::InjectPoint<ListInjectPoint2, Args...>();

			// From here, element cannot be used by any other concurrent operation,
			// as it has already been flagged as inserted.
			element->previous_.store(nodePrevious);

			bzd::test::InjectPoint<ListInjectPoint3, Args...>();

			{
				BasePtrType expected{nodeNext};
				if (!nodePrevious->next_.compareExchange(expected, element))
				{
					std::cout << "sakls" << std::endl;

					// The next node has been altered, retry.
					// Note the order here is important.
					{
						BasePtrType expected{nodePrevious};
						element->previous_.compareExchange(expected, nullptr);
					}
					{
						BasePtrType expected{nodeNext};
						element->next_.compareExchange(expected, nullptr);
					}
					element->next_.store(nullptr);
					continue;
				}
			}

			bzd::test::InjectPoint<ListInjectPoint4, Args...>();

			{
				BasePtrType expected{nodePrevious};
				if (!nodeNext->previous_.compareExchange(expected, element))
				{
					// Nothing needs to be done here, it means that the
					// pointer has been updated by another node.
					// Reverting things now will might lead to unconsitency between nodes.

					std::cout << "RACE! 1 insert" << std::endl;
					std::cout << nodePrevious << " <- " << element << " -> " << nodeNext << std::endl;
					std::cout << nodePrevious << " -> " << nodePrevious->next_.load() << " | ";
					std::cout << nodeNext->previous_.load() << " <- " << nodeNext << std::endl;

					// The next node has been altered, retry.
					// Note the order here is important.
					/*BasePtrType expected{element};
					nodePrevious->next_.compareExchange(expected, nodeNext);
					element->previous_.store(nullptr);
					element->next_.store(nullptr);

					continue;*/
					// break;

					// Should never happen
					// return makeError(ListErrorType::unhandledRaceCondition);
				}
			}

			bzd::test::InjectPoint<ListInjectPoint5, Args...>();

			// Remove busy mark
			{
				BasePtrType expected{addDeletionMark(nodeNext)};
				while (!element->next_.compareExchange(expected, removeDeletionMark(expected)))
				{
				}
			}

			break;
		}

		// Increase the element count.
		++size_;

		// Point to the next element if node is pointing to root.
		{
			BasePtrType expected{&head_};
			node_.compareExchange(expected, element);
		}

		return nullresult;
	}

	/**
	 * Remove an element from the queue.
	 */
	template <class... Args>
	Result<void> remove(PtrType element) noexcept
	{
		bzd::SizeType retry = 100000;

		// Add deletion mark
		{
			BasePtrType expected{removeDeletionMark(element->next_.load())};
			do
			{
				if (!expected || isDeletionMark(expected))
				{
					return makeError(ListErrorType::elementAlreadyRemoved);
				}
			} while (!element->next_.compareExchange(expected, addDeletionMark(expected)));
		}

		// At this point no other concurrent operation can use this element for removal nor insertion

		while (true)
		{
			if (--retry == 0)
			{
				std::cout << "ERROR" << std::endl;
				volatile char* tmo = nullptr;
				tmo[13] = 2;
			}

			// Save the previous and next positions of current element pointers
			auto nodePrevious = element->previous_.load();
			const auto nodeNext = removeDeletionMark(element->next_.load());

			bzd::test::InjectPoint<ListInjectPoint1, Args...>();

			// Look for the previous node.
			const auto previous = findPreviousNode(element, nodePrevious);
			if (!previous)
			{
				return makeError(ListErrorType::unhandledRaceCondition);
			}
			nodePrevious = previous->node;

			bzd::test::InjectPoint<ListInjectPoint2, Args...>();

			// Ensure that the pointers are valid
			if (!nodePrevious || !nodeNext)
			{
				std::cout << "null 3 " << nodePrevious << " " << nodeNext << std::endl;
				return makeError(ListErrorType::unhandledRaceCondition);
			}

			bzd::test::InjectPoint<ListInjectPoint3, Args...>();

			// Try to remove the right link
			{
				BasePtrType expected{previous->nextRaw};
				if (!nodePrevious->next_.compareExchange(expected,
														 (isDeletionMark(previous->nextRaw)) ? addDeletionMark(nodeNext) : nodeNext))
				{
					std::cout << "RACE! 2 remove" << std::endl;
					std::cout << nodePrevious << " <- " << element << " -> " << nodeNext << std::endl;
					std::cout << nodePrevious << " -> " << nodePrevious->next_.load() << " | ";
					std::cout << nodeNext->previous_.load() << " <- " << nodeNext << std::endl;

					// nodeNext has been deleted.
					// Try to revert the previously changed node, if it fails, discard it would have meant
					// that the previous has been altered.
					// expected = nodePrevious;
					// nodeNext->previous_.compareExchange(expected, element);

					continue;
				}
			}

			bzd::test::InjectPoint<ListInjectPoint4, Args...>();

			// Detach this element from the chain
			{
				BasePtrType expected{element};
				if (!nodeNext->previous_.compareExchange(expected, nodePrevious))
				{
					std::cout << "RACE! 1 remove" << std::endl;
					std::cout << nodePrevious << " <- " << element << " -> " << nodeNext << std::endl;
					std::cout << nodePrevious << " -> " << nodePrevious->next_.load() << " | ";
					std::cout << nodeNext->previous_.load() << " <- " << nodeNext << std::endl;

					// Check if necessary.
					// Idea is if it fails, it means a concurrent operation changed the pointer already.
					// continue;
				}
			}

			bzd::test::InjectPoint<ListInjectPoint5, Args...>();

			// Element is completly out of the chain
			// Note here the order is important, the next pointer should be updated
			// the last to make it insertable only at the end.
			element->previous_.store(nullptr);
			element->next_.store(nullptr);

			break;
		}

		// Decrease the element count.
		--size_;

		return nullresult;
	}

	/**
	 * Get the current element if any.
	 */
	constexpr bzd::Optional<T&> get() noexcept
	{
		auto ptr = node_.load();
		if (ptr == &head_)
		{
			return bzd::nullopt;
		}
		return reinterpret_cast<T&>(*ptr);
	}

	constexpr bzd::Optional<const T&> get() const noexcept
	{
		const auto ptr = node_.load();
		if (ptr == &head_)
		{
			return bzd::nullopt;
		}
		return reinterpret_cast<const T&>(*ptr);
	}

	/**
	 * Make the list point to the next element.
	 */
	constexpr void next() {}

	/**
	 * Ensures that all element are properly connected.
	 * \return The number of node evaluated.
	 */
	template <class U>
	Result<bzd::SizeType> sanityCheck(const U sanityCheckElement = [](const auto&) -> bool { return true; })
	{
		bzd::SizeType counter = 0;
		auto previous = &head_;
		auto node = removeDeletionMark(previous->next_.load());
		while (true)
		{
			// Ensure that the next pointer points to the current node.
			if (previous->next_.load() != node)
			{
				return makeError(ListErrorType::sanityCheck);
			}

			// Ensure that the previous pointers points to a previous node.
			{
				const auto result = findPreviousNode(node, node->previous_.load());
				if (!result)
				{
					return makeError(ListErrorType::sanityCheck);
				}
			}

			// Check if the end is reached.
			if (node == &tail_)
			{
				break;
			}

			// Check the element.
			{
				const bool result = sanityCheckElement(reinterpret_cast<T&>(*node));
				if (!result)
				{
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
			return makeError(ListErrorType::sanityCheck);
		}

		return counter;
	}

private:
	constexpr bool isDeletionMark(BasePtrType node) const noexcept { return (reinterpret_cast<IntPtrType>(node) & 1); }

	constexpr BasePtrType addDeletionMark(BasePtrType node) const noexcept
	{
		return reinterpret_cast<BasePtrType>(reinterpret_cast<IntPtrType>(node) | 0x1);
	}

	constexpr BasePtrType removeDeletionMark(BasePtrType node) const noexcept
	{
		return reinterpret_cast<BasePtrType>(reinterpret_cast<IntPtrType>(node) & ~1);
	}

private:
	// Having a head & tail elemment will simplify the logic to not have to take care of the edge cases.
	CircularListElement head_;
	CircularListElement tail_;
	// Number of elements.
	bzd::Atomic<bzd::SizeType> size_{0};
	bzd::Atomic<BasePtrType> node_{&head_};
};
} // namespace bzd::impl
