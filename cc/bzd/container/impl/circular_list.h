#pragma once

#include "bzd/container/span.h"
#include "bzd/core/assert/minimal.h"
#include "bzd/platform/types.h"

namespace bzd::impl {
class CircularListElement
{
public:
	CircularListElement* next_{};
	CircularListElement* previous_{};
};

/**
 * Implementation of a non-owning circular doubled linked list.
 * This implementation is thread safe.
 *
 * Implementation inspired by:
 * https://citeseerx.ist.psu.edu/viewdoc/download?doi=10.1.1.140.4693&rep=rep1&type=pdf
 * The main algorithm steps, see Figure 4, for inserting a new node at an arbitrary
position in our doubly linked list will thus be as follows: I) Atomically update the
next pointer of the to-be-previous node, II) Atomically update the prev pointer
of the to-be-next node. The main steps of the algorithm for deleting a node at an
arbitrary position are the following: I) Set the deletion mark on the next pointer
of the to-be-deleted node, II) Set the deletion mark on the prev pointer of the
to-be-deleted node, III) Atomically update the next pointer of the previous node
of the to-be-deleted node, IV) Atomically update the prev pointer of the next
node of the to-be-deleted node. As will be shown later in the detailed description
of the algorithm, helping techniques need to be applied in order to achieve the
lock-free property, following the same steps as the main algorithm for inserting
and deleting.
 */
template <class T>
class CircularList
{
private:
	using PtrType = T*;

public:
	constexpr SingleLinkedList() noexcept = default;

	/**
	 * Insert an element from the list.
	 * It will be insert just after the current element pointed by node_.
	 */
	void insert(PtrType element)
	{
		if (node_)
		{
			element->next_ = node_->next_;
			element->previous = node_;
			node_->next_ = element;
			node_->next->previous_ = element;
		}
		else
		{
			element->next_ = element;
			element->previous_ = element;
			node_ = element;
		}
	}

private:
	PtrType node_{};
};
} // namespace bzd::impl
