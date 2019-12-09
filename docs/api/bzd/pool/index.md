# [`bzd`](../../index.md)::[`Pool`](../index.md)

## `template<class T, SizeType N> class Pool`
A Pool is a fixed memory buffer containing fixed size elements that can be taken or released.
#### Template
||||
|---:|:---|:---|
|class|T||
|SizeType|N||

|Function||
|:---|:---|
|[`Pool()`](./index.md)||
|[`capacity() const`](./index.md)||
|[`empty() const`](./index.md)||
|[`release(T & container)`](./index.md)||
|[`reserve()`](./index.md)||
|[`toStream(std::ostream & os)`](./index.md)||
------
### `constexpr Pool()`

------
### `constexpr SizeType capacity() const`
*From bzd::impl::Pool*


------
### `constexpr bool empty() const`
*From bzd::impl::Pool*


------
### `constexpr void release(T & container)`
*From bzd::impl::Pool*

Release an element from the pool
#### Parameters
||||
|---:|:---|:---|
|T &|container||
------
### `constexpr T & reserve()`
*From bzd::impl::Pool*

Reserve an element from the free list (if any)
------
### `void toStream(std::ostream & os)`
*From bzd::impl::Pool*


#### Parameters
||||
|---:|:---|:---|
|std::ostream &|os||
