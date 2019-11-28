# [`bzd`](../../../../index.md)::[`format`](../../../index.md)::[`impl`](../../index.md)::[`ConstexprVector`](../index.md)

## `template<class T, SizeType N> class ConstexprVector`
Simple vector container working with conxtexpr
#### Template
||||
|---:|:---|:---|
|class|T||
|SizeType|N||

|Class||
|:---|:---|
|[`Iterator`](iterator/index.md)||

|Function||
|:---|:---|
|[`ConstexprVector()`](./index.md)||
|[`ConstexprVector(Args &&... args)`](./index.md)||
|[`begin() const`](./index.md)||
|[`end() const`](./index.md)||
|[`size() const`](./index.md)||
|[`capacity() const`](./index.md)||
|[`push_back(const T & element)`](./index.md)||
|[`operator[](const SizeType index)`](./index.md)||
|[`operator[](const SizeType index) const`](./index.md)||
------
### `class Iterator`

------
### `constexpr ConstexprVector()`

------
### `template<class... Args> constexpr ConstexprVector(Args &&... args)`

#### Template
||||
|---:|:---|:---|
|class...|Args||
#### Parameters
||||
|---:|:---|:---|
|Args &&...|args||
------
### `constexpr Iterator begin() const`

------
### `constexpr Iterator end() const`

------
### `constexpr SizeType size() const`

------
### `constexpr SizeType capacity() const`

------
### `constexpr void push_back(const T & element)`

#### Parameters
||||
|---:|:---|:---|
|const T &|element||
------
### `constexpr DataType & operator[](const SizeType index)`

#### Parameters
||||
|---:|:---|:---|
|const SizeType|index||
------
### `constexpr const DataType & operator[](const SizeType index) const`

#### Parameters
||||
|---:|:---|:---|
|const SizeType|index||
