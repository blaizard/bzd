# [`bzd`](../../index.md)::[`Pool`](../index.md)

## `template<class T, SizeType N> class Pool`
A Pool is a fixed memory buffer containing fixed size elements that can be taken or released.
#### Template
||||
|---:|:---|:---|
|class T|None||
|SizeType|N||
### Struct
||||
|---:|:---|:---|
|struct|[`ItemType`](./itemtype/index.md)||
### Function
||||
|---:|:---|:---|
|constexpr|[`Pool()`](.)||
|constexpr void|[`release(T * container)`](.)||
|constexpr T *|[`reserve()`](.)||
|void|[`toStream(std::ostream & os)`](.)||
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
