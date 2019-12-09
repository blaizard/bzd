# [`bzd`](../../../index.md)::[`impl`](../../index.md)::[`Pool`](../index.md)

## `template<class T> class Pool`

#### Template
||||
|---:|:---|:---|
|class|T||

|Function||
|:---|:---|
|[`Pool(const bzd::Span< Element > & data)`](./index.md)||
|[`capacity() const`](./index.md)||
|[`empty() const`](./index.md)||
|[`release(T & container)`](./index.md)||
|[`reserve()`](./index.md)||
|[`toStream(std::ostream & os)`](./index.md)||
------
### `constexpr Pool(const bzd::Span< Element > & data)`

#### Parameters
||||
|---:|:---|:---|
|const bzd::Span< Element > &|data||
------
### `constexpr SizeType capacity() const`

------
### `constexpr bool empty() const`

------
### `constexpr void release(T & container)`
Release an element from the pool
#### Parameters
||||
|---:|:---|:---|
|T &|container||
------
### `constexpr T & reserve()`
Reserve an element from the free list (if any)
------
### `void toStream(std::ostream & os)`

#### Parameters
||||
|---:|:---|:---|
|std::ostream &|os||
