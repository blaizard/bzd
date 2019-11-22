# [`bzd`](../../../index.md)::[`iterator`](../../index.md)::[`Contiguous`](../index.md)

## `template<class DataType> class Contiguous`

#### Template
||||
|---:|:---|:---|
|class DataType|None||
### Function
||||
|---:|:---|:---|
|constexpr|[`Contiguous(DataType * data, const SizeType index)`](./index.md)||
|constexpr SelfType &|[`operator++()`](./index.md)||
|constexpr SelfType|[`operator++(int)`](./index.md)||
|constexpr SelfType &|[`operator--()`](./index.md)||
|constexpr SelfType|[`operator--(int)`](./index.md)||
|constexpr SelfType|[`operator-(const int n) const`](./index.md)||
|constexpr SelfType|[`operator+(const int n) const`](./index.md)||
|constexpr SelfType &|[`operator-=(const int n)`](./index.md)||
|constexpr SelfType &|[`operator+=(const int n)`](./index.md)||
|constexpr bool|[`operator==(const SelfType & it) const`](./index.md)||
|constexpr bool|[`operator!=(const SelfType & it) const`](./index.md)||
|constexpr DataType &|[`operator*()`](./index.md)||
### Typedef
||||
|---:|:---|:---|
|typedef|[`SelfType`](./index.md)|alias of [`bzd::iterator::Contiguous`](./index.md)|
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

