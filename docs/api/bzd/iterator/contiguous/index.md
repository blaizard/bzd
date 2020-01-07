# [`bzd`](../../../index.md)::[`iterator`](../../index.md)::[`Contiguous`](../index.md)

## `template<class DataType> class Contiguous`

#### Template
||||
|---:|:---|:---|
|class DataType|None||

|Function||
|:---|:---|
|[`Contiguous(DataType * data, const SizeType index)`](./index.md)||
|[`operator!=(const SelfType & it) const`](./index.md)||
|[`operator*() const`](./index.md)||
|[`operator+(const int n) const`](./index.md)||
|[`operator++()`](./index.md)||
|[`operator++(int)`](./index.md)||
|[`operator+=(const int n)`](./index.md)||
|[`operator-(const int n) const`](./index.md)||
|[`operator--()`](./index.md)||
|[`operator--(int)`](./index.md)||
|[`operator-=(const int n)`](./index.md)||
|[`operator->() const`](./index.md)||
|[`operator==(const SelfType & it) const`](./index.md)||

|Typedef||
|:---|:---|
|[`SelfType`](./index.md)|alias of [`bzd::iterator::Contiguous`](./index.md)|
------
### `constexpr Contiguous(DataType * data, const SizeType index)`

#### Parameters
||||
|---:|:---|:---|
|DataType *|data||
|const SizeType|index||
------
### `constexpr bool operator!=(const SelfType & it) const`

#### Parameters
||||
|---:|:---|:---|
|const SelfType &|it||
------
### `constexpr DataType & operator*() const`

------
### `constexpr SelfType operator+(const int n) const`

#### Parameters
||||
|---:|:---|:---|
|const int|n||
------
### `constexpr SelfType & operator++()`

------
### `constexpr SelfType operator++(int)`

#### Parameters
||||
|---:|:---|:---|
|int|None||
------
### `constexpr SelfType & operator+=(const int n)`

#### Parameters
||||
|---:|:---|:---|
|const int|n||
------
### `constexpr SelfType operator-(const int n) const`

#### Parameters
||||
|---:|:---|:---|
|const int|n||
------
### `constexpr SelfType & operator--()`

------
### `constexpr SelfType operator--(int)`

#### Parameters
||||
|---:|:---|:---|
|int|None||
------
### `constexpr SelfType & operator-=(const int n)`

#### Parameters
||||
|---:|:---|:---|
|const int|n||
------
### `constexpr DataType * operator->() const`

------
### `constexpr bool operator==(const SelfType & it) const`

#### Parameters
||||
|---:|:---|:---|
|const SelfType &|it||
------
### `typedef SelfType`

