# [`bzd`](../../../../../index.md)::[`format`](../../../../index.md)::[`impl`](../../../index.md)::[`ConstexprVector`](../../index.md)::[`Iterator`](../index.md)

## `class Iterator`

### Function
||||
|---:|:---|:---|
|constexpr|[`Iterator(const SelfType & container, const SizeType index)`](.)||
|constexpr Iterator &|[`operator++()`](.)||
|constexpr bool|[`operator==(const Iterator & it) const`](.)||
|constexpr bool|[`operator!=(const Iterator & it) const`](.)||
|constexpr const DataType &|[`operator*() const`](.)||
------
### `constexpr Iterator(const SelfType & container, const SizeType index)`

#### Parameters
||||
|---:|:---|:---|
|const SelfType &|container||
|const SizeType|index||
------
### `constexpr Iterator & operator++()`

------
### `constexpr bool operator==(const Iterator & it) const`

#### Parameters
||||
|---:|:---|:---|
|const Iterator &|it||
------
### `constexpr bool operator!=(const Iterator & it) const`

#### Parameters
||||
|---:|:---|:---|
|const Iterator &|it||
------
### `constexpr const DataType & operator*() const`

