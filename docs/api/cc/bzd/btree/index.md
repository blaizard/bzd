# [`bzd`](../../index.md)::[`BTree`](../index.md)

## `template<class K, class V, SizeType N, SizeType Order> class BTree`

#### Template
||||
|---:|:---|:---|
|class K|None||
|class V|None||
|SizeType|N||
|SizeType|Order||

|Function||
|:---|:---|
|[`BTree()`](./index.md)||
|[`insert(const K & key, const V &)`](./index.md)||
------
### `constexpr BTree()`

------
### `void insert(const K & key, const V &)`
*From bzd::impl::BTree*

Insert a new element into the B-Tree.

Inserting into a B-Tree of order M is done as follow:Find the leaf node where the item should be inserted.If the leaf node can accomodate another item (it has no more than M - 1 items), insert the itme into the correct location in the node.If the leaf node is full, split the node in 2, with the smaller half of the items in one node and the larger half in the other. "Promote" the median item to the parent node. If the parent node is full, split and repeat...
#### Parameters
||||
|---:|:---|:---|
|const K &|key||
|const V &|None||
