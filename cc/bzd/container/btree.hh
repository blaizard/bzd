#pragma once

#include "cc/bzd/container/pool.hh"
#include "cc/bzd/container/tuple.hh"
#include "cc/bzd/container/vector.hh"
#include "cc/bzd/platform/types.hh"

namespace bzd::impl {
/// Implementation of a B-tree.
///
/// A B-Tree of order M has the following properties:
/// - Every nodes has at most *M* children.
/// - A non-leaf node with k children contains k-1 keys.
/// - The root has at least 2 children if it is not a leaf node.
/// - Every non-leaf node (except root) has at least M / 2 children.
/// - All leaves appear in the same level.
///
/// Non-leaf nodes are implemented as non-sorted arrays.
template <class K, class V, SizeType Order>
class BTree
{
protected:
	struct Node
	{
		bzd::Vector<bzd::Tuple<K, Node*>, Order - 1> keys_;
		Node* last_;
		// bzd::Vector<Node*, Order> next_;
	};

public:
	constexpr explicit BTree(const SizeType capacity, Node** root, bzd::interface::Pool<Node>& nodes) :
		capacity_(capacity), root_(root), nodes_(nodes)
	{
	}

	/**
	 * Insert a new element into the B-Tree.
	 *
	 * Inserting into a B-Tree of order M is done as follow:
	 * 1. Find the leaf node where the item should be inserted.
	 * 2. If the leaf node can accomodate another item (it has no more than M - 1
	 * items), insert the itme into the correct location in the node.
	 * 3. If the leaf node is full, split the node in 2, with the smaller half of
	 * the items in one node and the larger half in the other. "Promote" the
	 * median item to the parent node. If the parent node is full, split and
	 * repeat...
	 */
	void insert(const K& key, const V& /*value*/)
	{
		Node* node = *root_;

		// If there is no node yet, create one
		if (!*root_)
		{
			*root_ = &nodes_.reserve();
			node = *root_;
		}

		// Look for the right node
		Node* subNode = nullptr;
		for (const auto& nodeKey : node->keys_)
		{
			if (key >= nodeKey.template get<0>())
			{
				subNode = nodeKey.template get<1>();
			}
		}
		if (!subNode)
		{
			subNode = node->last_;
		}

		// If the current node is full, create a new one
		if (node->keys_.size() == node->keys_.capacity())
		{
		}

		node->keys_.pushBack(key);
	}

protected:
	const SizeType capacity_;
	Node** root_;
	bzd::interface::Pool<Node>& nodes_;
};
} // namespace bzd::impl

namespace bzd::interface {
template <class K, class V, SizeType Order>
using BTree = impl::BTree<K, V, Order>;
}

namespace bzd {
template <class K, class V, SizeType N, SizeType Order>
class BTree : public interface::BTree<K, V, Order>
{
private:
	using typename interface::BTree<K, V, Order>::Node;

public:
	constexpr BTree() : interface::BTree<K, V, Order>(N, &root_, nodes_) {}

protected:
	bzd::Pool<Node, 10> nodes_;
	Node* root_ = nullptr;
	V data_[N];
};
} // namespace bzd
