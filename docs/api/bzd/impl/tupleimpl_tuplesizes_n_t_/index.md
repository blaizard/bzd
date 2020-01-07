# [`bzd`](../../../index.md)::[`impl`](../../index.md)::[`TupleImpl< TupleSizes< N... >, T... >`](../index.md)

## `template<SizeType... N, class... T> class TupleImpl< TupleSizes< N... >, T... >`

#### Template
||||
|---:|:---|:---|
|SizeType...|N||
|class...|T||

|Function||
|:---|:---|
|[`get()`](./index.md)||
|[`get() const`](./index.md)||
|[`get()`](./index.md)||
|[`get() const`](./index.md)||
|[`TupleElem()`](./index.md)||
|[`TupleElem(const T & value)`](./index.md)||
|[`TupleElem(const NoType &)`](./index.md)||
|[`TupleImpl(Args &&... args)`](./index.md)||
------
### `template<SizeType M> constexpr pick< M > & get()`

#### Template
||||
|---:|:---|:---|
|SizeType|M||
------
### `template<SizeType M> constexpr const pick< M > & get() const`

#### Template
||||
|---:|:---|:---|
|SizeType|M||
------
### `constexpr T & get()`
*From bzd::impl::TupleElem*


------
### `constexpr const T & get() const`
*From bzd::impl::TupleElem*


------
### `constexpr TupleElem()`
*From bzd::impl::TupleElem*


------
### `constexpr TupleElem(const T & value)`
*From bzd::impl::TupleElem*


#### Parameters
||||
|---:|:---|:---|
|const T &|value||
------
### `constexpr TupleElem(const NoType &)`
*From bzd::impl::TupleElem*


#### Parameters
||||
|---:|:---|:---|
|const NoType &|None||
------
### `template<class... Args> constexpr TupleImpl(Args &&... args)`

#### Template
||||
|---:|:---|:---|
|class...|Args||
#### Parameters
||||
|---:|:---|:---|
|Args &&...|args||
