#pragma once

#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/reference_wrapper.hh"
#include "cc/bzd/container/result.hh"
#include "cc/bzd/core/assert/minimal.hh"
#include "cc/bzd/platform/atomic.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/test/inject_point.hh"
#include "cc/bzd/type_traits/add_const.hh"
#include "cc/bzd/type_traits/conditional.hh"
#include "cc/bzd/type_traits/is_const.hh"
#include "cc/bzd/type_traits/iterator.hh"
#include "cc/bzd/type_traits/remove_reference.hh"
#include "cc/bzd/utility/ignore.hh"
#include "cc/bzd/utility/scope_guard.hh"

namespace bzd::threadsafe {
enum class NonOwningForwardListErrorType
{
	elementAlreadyInserted,
	elementAlreadyRemoved,
	notFound,
};
}

namespace bzd::threadsafe::impl {

class NonOwningForwardListNoDiscard
{
protected:
	constexpr auto discardCounterScope() const noexcept { return true; }
};

class NonOwningForwardListDiscard
{
public:
	constexpr void waitToDiscard() noexcept
	{
		while (discardCounter_.load(MemoryOrder::acquire) != 0)
		{
		}
	}

public:
	constexpr auto discardCounterScope() const noexcept
	{
		discardCounter_.fetchAdd(1, MemoryOrder::release);
		return bzd::ScopeGuard{[this]() { discardCounter_.fetchSub(1, MemoryOrder::release); }};
	}

private:
	// When this counter is non-null it means that discarding is not possible.
	mutable bzd::Atomic<bzd::Size> discardCounter_{0};
};

/// Implementation of a non-owning forward list.
///
/// Lock-free, multi-producer, multi-consumer.
/// In addition it supports insertion and deletion from preemptive OS
/// within a critical section.
/// Elements can be moved from one list instance to another.
///
/// \tparam T Element type.
template <class T>
class NonOwningForwardList : public typeTraits::Conditional<T::supportDiscard_, NonOwningForwardListDiscard, NonOwningForwardListNoDiscard>
{
public:
	using Self = NonOwningForwardList<T>;
	using ElementType = T;
	using ElementPtrType = ElementType*;

	template <class V>
	using Result = bzd::Result<V, NonOwningForwardListErrorType>;

public:
	constexpr NonOwningForwardList() noexcept { front_.next_.store(&back_); }

	/// Get the number of elements currently held by this container.
	///
	/// \return The number of elments.
	[[nodiscard]] constexpr Size size() const noexcept { return size_.load(); }

	/// Tells if the list is empty.
	///
	/// \return True if the list is empty, false otherwise.
	[[nodiscard]] constexpr bool empty() const noexcept { return size_.load() == 0; }

	/// Insert an element into the list from the root element.
	/// The idea is to ensure that the last operation is the one that make the element discoverable,
	/// this ensures that any element is consistent.
	///
	/// Given the following:
	/// | R | -> | A |
	///
	/// To insert element B, first set the next pointer of this element:
	/// | R | --------> | A |
	/// |   |  | B | -> |   |
	///
	/// Then update R next pointer to B, if it fails restart from the begining.
	/// | R | -> | B | -> | A |
	///
	/// \param element Element to be inserted.
	/// \return An error in case of failure, void otherwise.
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
					return bzd::error::make(NonOwningForwardListErrorType::elementAlreadyInserted);
				}
			}

			// From here, element cannot be used by any other concurrent operation,
			// as it has already been flagged as inserted.

			bzd::test::InjectPoint<bzd::test::InjectPoint1, Args...>();

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

			bzd::test::InjectPoint<bzd::test::InjectPoint2, Args...>();

			if constexpr (ElementType::supportMultiContainer_)
			{
				// Set the parent
				element.parent_.store(this);
			}

			bzd::test::InjectPoint<bzd::test::InjectPoint3, Args...>();

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

	/// Remove an element from the queue.
	///
	/// \param element Element to be inserted.
	/// \return An error in case of failure, void otherwise.
	template <class... Args>
	[[nodiscard]] constexpr Result<void> pop(ElementType& element) noexcept
	{
		// Add deletion mark, this must be done in 2 parts.
		// First set a temporary mark and check wether or not this element
		// is part of this list. If not, remove the mark and return, otherwise
		// replace the mark with the deletion mark.
		if constexpr (ElementType::supportMultiContainer_)
		{
			ElementPtrType expected{element.next_.load()};
			do
			{
				if (!expected || isDeletionMark(expected) || isInsertionMark(expected))
				{
					return bzd::error::make(NonOwningForwardListErrorType::elementAlreadyRemoved);
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

				return bzd::error::make(NonOwningForwardListErrorType::notFound);
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
					return bzd::error::make(NonOwningForwardListErrorType::elementAlreadyRemoved);
				}
			} while (!element.next_.compareExchange(expected, setDeletionMark(expected)));
		}

		// At this point no other concurrent operation can use this element for removal nor insertion

		while (true)
		{
			[[maybe_unused]] auto scope{this->discardCounterScope()};

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
				bzd::Size distanceFromElement{1};
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

	/// Remove an element from the queue. Unlike pop, the element can be immediatly discarded.
	///
	/// Calling this function ensures that no user of the list is currently pointing to this element,
	/// which could lead to a sigfault situation.
	///
	/// \param element Element to be inserted.
	/// \return An error in case of failure, void otherwise.
	template <bzd::Bool U = ElementType::supportDiscard_, bzd::typeTraits::EnableIf<U, void>* = nullptr>
	[[nodiscard]] constexpr Result<void> popToDiscard(ElementType& element) noexcept
	{
		const auto result = pop(element);
		if (result)
		{
			this->waitToDiscard();
		}
		return result;
	}

	/// Remove all elements associated with this list.
	constexpr void clear() noexcept
	{
		while (true)
		{
			auto ptr = front_.next_.load();
			if (ptr == &this->back_)
			{
				break;
			}
			bzd::ignore = pop(*ptr);
		}
	}

protected:
	/// Structure to return node information when found.
	struct NodeFound
	{
		/// Pointer of the previous node.
		ElementPtrType node;
		/// Raw pointer of the next pointer of the previous node.
		/// Raw means without removing deletion mark.
		ElementPtrType nextRaw;
	};

	/// Look for the previous node of the one passed into argument.
	/// In case of concurrent insertion, the previous node might not be the
	/// actual one, therefore we need to go down the chain to find the actual one.
	///
	/// \param node We are looking for the previous node of this node.
	/// \return A structure containing the previous node and the raw pointer its next node.
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
	[[nodiscard]] static constexpr bool isDeletionMark(ElementPtrType node) noexcept
	{
		return (reinterpret_cast<IntPointer>(node) & 3) == 1;
	}
	[[nodiscard]] static constexpr bool isInsertionMark(ElementPtrType node) noexcept
	{
		return (reinterpret_cast<IntPointer>(node) & 3) == 2;
	}
	[[nodiscard]] static constexpr bool isWeakMark(ElementPtrType node) noexcept { return (reinterpret_cast<IntPointer>(node) & 3) == 3; }

	[[nodiscard]] static constexpr ElementPtrType setDeletionMark(ElementPtrType node) noexcept
	{
		return reinterpret_cast<ElementPtrType>(reinterpret_cast<IntPointer>(removeMarks(node)) | 0x1);
	}

	[[nodiscard]] static constexpr ElementPtrType setInsertionMark(ElementPtrType node) noexcept
	{
		return reinterpret_cast<ElementPtrType>(reinterpret_cast<IntPointer>(removeMarks(node)) | 0x2);
	}

	[[nodiscard]] static constexpr ElementPtrType setWeakMark(ElementPtrType node) noexcept
	{
		return reinterpret_cast<ElementPtrType>(reinterpret_cast<IntPointer>(removeMarks(node)) | 0x3);
	}

	[[nodiscard]] static constexpr ElementPtrType removeMarks(ElementPtrType node) noexcept
	{
		return reinterpret_cast<ElementPtrType>(reinterpret_cast<IntPointer>(node) & ~3);
	}

protected:
	// Having a front & back elemment will simplify the logic to not have to take care of the edge cases.
	ElementType front_;
	ElementType back_;
	// Number of elements.
	bzd::Atomic<bzd::Size> size_{0};
};

// ---- NonOwningForwardListElement

struct NonOwningForwardListElementVoid
{
};

struct NonOwningForwardListElementMultiContainer
{
	// Pointer to the current container.
	bzd::Atomic<void*> parent_{nullptr};
};

template <bzd::Bool supportMultiContainer, bzd::Bool supportDiscard>
class NonOwningForwardListElement
	: public bzd::typeTraits::Conditional<supportMultiContainer, NonOwningForwardListElementMultiContainer, NonOwningForwardListElementVoid>
{
public:
	static const constexpr bzd::Bool supportMultiContainer_{supportMultiContainer};
	static const constexpr bzd::Bool supportDiscard_{supportDiscard};

private:
	using Self = NonOwningForwardListElement<supportMultiContainer, supportDiscard>;
	using Container = NonOwningForwardList<Self>;
	template <class V>
	using Result = bzd::Result<V, NonOwningForwardListErrorType>;

public:
	constexpr NonOwningForwardListElement() noexcept = default;
	constexpr NonOwningForwardListElement(Self&& elt) noexcept : next_{elt.next_.load()} {}

	/// Pop the current element from the list. This is available only if
	/// supportMultiContainer is enabled, as it has the knowledge of the parent container.
	template <bzd::Bool T = supportMultiContainer, bzd::typeTraits::EnableIf<T, void>* = nullptr>
	[[nodiscard]] constexpr Result<void> pop() noexcept
	{
		auto* container = static_cast<Container*>(this->parent_.load());
		if (container)
		{
			return container->pop(*this);
		}
		return bzd::error::make(NonOwningForwardListErrorType::elementAlreadyRemoved);
	}

	/// Pop the current element from the list. This is available only if
	/// supportMultiContainer and supportDiscard is enabled, as it has the knowledge of the parent container.
	template <bzd::Bool T = (supportMultiContainer && supportDiscard), bzd::typeTraits::EnableIf<T, void>* = nullptr>
	[[nodiscard]] constexpr Result<void> popToDiscard() noexcept
	{
		auto* container = static_cast<Container*>(this->parent_.load());
		if (container)
		{
			return container->popToDiscard(*this);
		}
		return bzd::error::make(NonOwningForwardListErrorType::elementAlreadyRemoved);
	}

	// Pointer to the next element from the list.
	bzd::Atomic<Self*> next_{nullptr};
};
} // namespace bzd::threadsafe::impl

namespace bzd::threadsafe {

/// Element for the non-owning list.
///
/// \tparam supportMultiContainer Whether multicontainer should be supported or not.
///        This means that an element can pop from one list and push a different one.
/// \tparam supportDiscard Allow elements to be discarded.
template <bzd::Bool supportMultiContainer, bzd::Bool supportDiscard = false>
using NonOwningForwardListElement = bzd::threadsafe::impl::NonOwningForwardListElement<supportMultiContainer, supportDiscard>;

/// Implementation of a non-owning linked list.
///
/// The implementation is based on a single linked list, which should be efficient
/// enough for insertion and removal for small lists.
///
/// Lock-free, multi-producer, multi-consumer.
/// In addition it supports lock-free insertion and deletion from critical section,
/// meaning that a call will always return without the need of task preemption.
/// Elements can be moved from one list instance to another.
///
/// \tparam T Element type.
template <class T>
class NonOwningForwardList
	: public bzd::threadsafe::impl::NonOwningForwardList<
		  bzd::threadsafe::NonOwningForwardListElement<T::supportMultiContainer_, T::supportDiscard_>>
{
public:
	using Parent = bzd::threadsafe::impl::NonOwningForwardList<
		bzd::threadsafe::NonOwningForwardListElement<T::supportMultiContainer_, T::supportDiscard_>>;

public:
	template <class U>
	class NonOwningForwardListIterator
	{
	public: // Traits
		using Self = NonOwningForwardListIterator<U>;
		using Category = bzd::typeTraits::ForwardTag;
		using IndexType = bzd::Size;
		using DifferenceType = bzd::Int32;
		using ValueType = U;

	private: // Internal Traits.
		using ElementType = typename Parent::ElementType;
		using UnderlyingValuePtrType =
			typeTraits::Conditional<typeTraits::isConst<ValueType>, typeTraits::AddConst<ElementType>, ElementType>*;

	public:
		constexpr NonOwningForwardListIterator(UnderlyingValuePtrType ptr) noexcept : current_{ptr} {}

	public: // Modifiers.
		constexpr Self& operator++() noexcept
		{
			next();
			return *this;
		}

		constexpr Self operator++(int) noexcept
		{
			Self it{*this};
			++(*this);
			return it;
		}

	public: // Comparators.
		[[nodiscard]] constexpr bool operator==(const Self& other) const noexcept { return (current_ == other.current_); }
		[[nodiscard]] constexpr bool operator!=(const Self& other) const noexcept { return !(other == *this); }

	public: // Accessors.
		[[nodiscard]] constexpr ValueType& operator*() const noexcept { return *static_cast<ValueType*>(current_); }
		[[nodiscard]] constexpr ValueType* operator->() const noexcept { return static_cast<ValueType*>(current_); }

	private:
		constexpr void next() noexcept
		{
			auto nextRaw = current_->next_.load();
			current_ = Parent::removeMarks(nextRaw);
			bzd::assert::isTrue(current_);
		}

	private:
		UnderlyingValuePtrType current_;
	};

public: // Traits.
	using Iterator = NonOwningForwardListIterator<T>;
	using ConstIterator = NonOwningForwardListIterator<typeTraits::AddConst<T>>;

public: // Return type.
	template <class U, class Scope = Bool>
	class ElementScope
	{
	public:
		constexpr explicit ElementScope(U& element) noexcept : element_{element} {}
		constexpr ElementScope(U& element, Scope&& scope) noexcept : element_{element}, scope_{bzd::move(scope)} {}

	public:
		[[nodiscard]] constexpr U& get() const noexcept { return element_; }

	private:
		U& element_;
		typeTraits::RemoveReference<Scope> scope_{false};
	};

public:
	using bzd::threadsafe::impl::NonOwningForwardList<
		bzd::threadsafe::NonOwningForwardListElement<T::supportMultiContainer_, T::supportDiscard_>>::NonOwningForwardList;

	/// Return a begin iterator for this list.
	///
	/// The iterator is not thread safe unlike the rest of this class, therefore iteration
	/// should be protected by a mutex.
	[[nodiscard]] constexpr auto begin() noexcept { return ++Iterator{&this->front_}; }
	[[nodiscard]] constexpr auto begin() const noexcept { return ++ConstIterator{&this->front_}; }

	/// Return an end iterator for this list.
	[[nodiscard]] constexpr auto end() noexcept { return Iterator{&this->back_}; }
	[[nodiscard]] constexpr auto end() const noexcept { return ConstIterator{&this->back_}; }

	[[nodiscard]] constexpr bzd::Optional<ElementScope<T>> front() noexcept
	{
		auto ptr = this->front_.next_.load();
		if (ptr == &this->back_)
		{
			return bzd::nullopt;
		}
		return ElementScope<T>{static_cast<T&>(*ptr)};
	}

	[[nodiscard]] constexpr bzd::Optional<ElementScope<const T>> front() const noexcept
	{
		const auto ptr = this->front_.next_.load();
		if (ptr == &this->back_)
		{
			return bzd::nullopt;
		}
		return ElementScope<const T>{static_cast<const T&>(*ptr)};
	}

	[[nodiscard]] constexpr auto back() noexcept -> bzd::Optional<ElementScope<T, decltype(this->discardCounterScope())>>
	{
		auto scope{this->discardCounterScope()};

		const auto previous = this->findPreviousNode(&this->back_);
		if (previous->node == &this->front_)
		{
			return bzd::nullopt;
		}
		return ElementScope<T, decltype(scope)>{static_cast<T&>(*previous->node), bzd::move(scope)};
	}

	[[nodiscard]] constexpr auto back() const noexcept -> bzd::Optional<ElementScope<const T, decltype(this->discardCounterScope())>>
	{
		auto scope{this->discardCounterScope()};

		const auto previous = this->findPreviousNode(&this->back_);
		if (previous->node == &this->front_)
		{
			return bzd::nullopt;
		}
		return ElementScope<const T, decltype(scope)>{static_cast<const T&>(*previous->node), bzd::move(scope)};
	}
};
} // namespace bzd::threadsafe
