# [`bzd`](../../../index.md)::[`impl`](../../index.md)::[`Vector`](../index.md)

## `template<class T, class Impl> class Vector`

#### Template
||||
|---:|:---|:---|
|class|T||
|class|Impl||

|Function||
|:---|:---|
|[`Vector(const SizeType capacity, Args &&... args)`](./index.md)||
|[`pushBack(T && value)`](./index.md)|Adds a new element at the end of the vector, after its current last element.|
|[`capacity() const`](./index.md)|Returns the maximum number of elements the vector can hold.|
|[`clear()`](./index.md)|Removes all elements.|
|[`resize(const bzd::SizeType n)`](./index.md)|Change the size of the vector.|
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
### `constexpr void pushBack(T && value)`
Adds a new element at the end of the vector, after its current last element.

The content of val is copied (or moved) to the new element.
#### Parameters
||||
|---:|:---|:---|
|T &&|value|Value to be copied (or moved) to the new element. |
------
### `constexpr SizeType capacity() const`
Returns the maximum number of elements the vector can hold.

Maximum number of element this vector can hold.
------
### `constexpr void clear()`
Removes all elements.
------
### `constexpr void resize(const bzd::SizeType n)`
Change the size of the vector.
#### Parameters
||||
|---:|:---|:---|
|const bzd::SizeType|n|The new size. Note, it must a be lower or equal to the capacity. |
