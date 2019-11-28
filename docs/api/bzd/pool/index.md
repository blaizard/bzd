# [`bzd`](../../index.md)::[`Pool`](../index.md)

## `template<class T, SizeType N> class Pool`
A Pool is a fixed memory buffer containing fixed size elements that can be taken or released.
#### Template
||||
|---:|:---|:---|
|class T|None||
|SizeType|N||

|Struct||
|:---|:---|
|[`ItemType`](itemtype/index.md)||

|Function||
|:---|:---|
|[`Pool()`](./index.md)||
|[`release(T * container)`](./index.md)||
|[`reserve()`](./index.md)||
|[`toStream(std::ostream & os)`](./index.md)||
------
### `struct ItemType`

------
### `constexpr Pool()`

------
### `constexpr void release(T * container)`
Release an element from the pool
#### Parameters
||||
|---:|:---|:---|
|T *|container||
------
### `constexpr T * reserve()`
Reserve an element from the free list (if any)
------
### `void toStream(std::ostream & os)`

#### Parameters
||||
|---:|:---|:---|
|std::ostream &|os||
