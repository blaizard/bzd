# [`bzd`](../../../index.md)::[`iterator`](../../index.md)::[`Contiguous`](../index.md)

## `template<class DataType> class Contiguous`

#### Template
||||
|---:|:---|:---|
|class DataType|None||
### Function
||||
|---:|:---|:---|
|constexpr|[`Contiguous(DataType * data, const SizeType index)`](.)||
|constexpr SelfType &|[`operator++()`](.)||
|constexpr SelfType|[`operator++(int)`](.)||
|constexpr SelfType &|[`operator--()`](.)||
|constexpr SelfType|[`operator--(int)`](.)||
|constexpr SelfType|[`operator-(const int n) const`](.)||
|constexpr SelfType|[`operator+(const int n) const`](.)||
|constexpr SelfType &|[`operator-=(const int n)`](.)||
|constexpr SelfType &|[`operator+=(const int n)`](.)||
|constexpr bool|[`operator==(const SelfType & it) const`](.)||
|constexpr bool|[`operator!=(const SelfType & it) const`](.)||
|constexpr DataType &|[`operator*()`](.)||
### Typedef
||||
|---:|:---|:---|
|typedef|[`SelfType`](.)|alias of [`bzd::iterator::Contiguous`](./index.md)|
------
### `constexpr Contiguous(DataType * data, const SizeType index)`

#### Parameters
||||
|---:|:---|:---|
|DataType *|data||
|const SizeType|index||
------
### `constexpr SelfType & operator++()`

------
### `constexpr SelfType operator++(int)`

#### Parameters
||||
|---:|:---|:---|
|int|None||
------
### `constexpr SelfType & operator--()`

------
### `constexpr SelfType operator--(int)`

#### Parameters
||||
|---:|:---|:---|
|int|None||
------
### `constexpr SelfType operator-(const int n) const`

#### Parameters
||||
|---:|:---|:---|
|const int|n||
------
### `constexpr SelfType operator+(const int n) const`

#### Parameters
||||
|---:|:---|:---|
|const int|n||
------
### `constexpr SelfType & operator-=(const int n)`

#### Parameters
||||
|---:|:---|:---|
|const int|n||
------
### `constexpr SelfType & operator+=(const int n)`

#### Parameters
||||
|---:|:---|:---|
|const int|n||
------
### `constexpr bool operator==(const SelfType & it) const`

#### Parameters
||||
|---:|:---|:---|
|const SelfType &|it||
------
### `constexpr bool operator!=(const SelfType & it) const`

#### Parameters
||||
|---:|:---|:---|
|const SelfType &|it||
------
### `constexpr DataType & operator*()`

------
### `typedef SelfType`

