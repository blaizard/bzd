# [`bzd`](../../../index.md)::[`impl`](../../index.md)::[`Vector`](../index.md)

## `template<class T, class Impl> class Vector`

#### Template
||||
|---:|:---|:---|
|class|T||
|class|Impl||
### Function
||||
|---:|:---|:---|
|constexpr|[`Vector(const SizeType capacity, Args &&... args)`](./index.md)||
|constexpr void|[`pushBack(const T & element)`](./index.md)|Adds a new element at the end of the vector, after its current last element.|
|constexpr SizeType|[`capacity() const`](./index.md)|Returns the maximum number of elements the vector can hold.|
|constexpr void|[`clear()`](./index.md)|Removes all elements.|
|constexpr void|[`resize(const size_t n)`](./index.md)|Change the size of the vector.|
------
### `template<class... Args> explicit constexpr Vector(const SizeType capacity, Args &&... args)`

#### Template
||||
|---:|:---|:---|
|class...|Args||
#### Parameters
||||
|---:|:---|:---|
|const SizeType|capacity||
|Args &&...|args||
------
### `constexpr void pushBack(const T & element)`
Adds a new element at the end of the vector, after its current last element.

The content of val is copied (or moved) to the new element.
#### Parameters
||||
|---:|:---|:---|
|const T &|element|Value to be copied (or moved) to the new element. |
------
### `constexpr SizeType capacity() const`
Returns the maximum number of elements the vector can hold.

Maximum number of element this vector can hold.
------
### `constexpr void clear()`
Removes all elements.
------
### `constexpr void resize(const size_t n)`
Change the size of the vector.
#### Parameters
||||
|---:|:---|:---|
|const size_t|n|The new size. Note, it must a be lower or equal to the capacity. |
