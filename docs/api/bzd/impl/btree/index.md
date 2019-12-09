# [`bzd`](../../../index.md)::[`impl`](../../index.md)::[`BTree`](../index.md)

## `template<class K, class V, SizeType Order> class BTree`
Implementation of a B-tree.

A B-Tree of order M has the following properties:Every nodes has at most M children.A non-leaf node with k children contains k-1 keys.The root has at least 2 children if it is not a leaf node.Every non-leaf node (except root) has at least M / 2 children.All leaves appear in the same level.

Non-leaf nodes are implemented as non-sorted arrays.
#### Template
||||
|---:|:---|:---|
|class K|None||
|class V|None||
|SizeType|Order||

|Function||
|:---|:---|
|[`BTree(const SizeType capacity, Node ** root, bzd::interface::Pool< Node > & nodes)`](./index.md)||
|[`insert(const K & key, const V &)`](./index.md)||
------
### `explicit constexpr BTree(const SizeType capacity, Node ** root, bzd::interface::Pool< Node > & nodes)`

#### Parameters
||||
|---:|:---|:---|
|const SizeType|capacity||
|Node **|root||
|bzd::interface::Pool< Node > &|nodes||
------
### `void insert(const K & key, const V &)`
Insert a new element into the B-Tree.

Inserting into a B-Tree of order M is done as follow:Find the leaf node where the item should be inserted.If the leaf node can accomodate another item (it has no more than M - 1 items), insert the itme into the correct location in the node.If the leaf node is full, split the node in 2, with the smaller half of the items in one node and the larger half in the other. "Promote" the median item to the parent node. If the parent node is full, split and repeat...
#### Parameters
||||
|---:|:---|:---|
|const K &|key||
|const V &|None||
