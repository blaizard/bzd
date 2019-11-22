# [`bzd`](../../../index.md)::[`impl`](../../index.md)::[`TupleImpl< TupleSizes< N... >, T... >`](../index.md)

## `template<SizeType... N, class... T> class TupleImpl< TupleSizes< N... >, T... >`

#### Template
||||
|---:|:---|:---|
|SizeType...|N||
|class...|T||
### Function
||||
|---:|:---|:---|
|constexpr|[`TupleImpl(Args &&... args)`](.)||
|constexpr pick< M > &|[`get()`](.)||
|constexpr const pick< M > &|[`get() const`](.)||
|constexpr|[`TupleElem()`](.)||
|constexpr|[`TupleElem(const T & value)`](.)||
|constexpr|[`TupleElem(const NoType &)`](.)||
|constexpr T &|[`get()`](.)||
|constexpr const T &|[`get() const`](.)||
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
### `constexpr T & get()`
*From bzd::impl::TupleElem*


------
### `constexpr const T & get() const`
*From bzd::impl::TupleElem*


