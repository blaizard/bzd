# [`bzd`](../../../index.md)::[`impl`](../../index.md)::[`String`](../index.md)

## `template<class T, class Impl> class String`

#### Template
||||
|---:|:---|:---|
|class T|None||
|class Impl|None||
### Function
||||
|---:|:---|:---|
|constexpr|[`String(const SizeType capacity, Args &&... args)`](./index.md)||
|constexpr SizeType|[`append(const StringView & str)`](./index.md)||
|constexpr SizeType|[`append(const T c)`](./index.md)||
|constexpr SizeType|[`append(const T * data, const SizeType n)`](./index.md)||
|constexpr SizeType|[`append(const SizeType n, const T c)`](./index.md)||
|constexpr SizeType|[`assign(const U & data)`](./index.md)||
|constexpr SizeType|[`capacity() const`](./index.md)||
|constexpr void|[`clear()`](./index.md)||
|constexpr void|[`resize(const SizeType n)`](./index.md)||
|constexpr String< T, Impl > &|[`operator+=(const U & data)`](./index.md)||
|constexpr String< T, Impl > &|[`operator=(const U & data)`](./index.md)||
### Variable
||||
|---:|:---|:---|
|const SizeType|[`capacity_`](./index.md)||
------
### `template<class... Args> explicit constexpr String(const SizeType capacity, Args &&... args)`

#### Template
||||
|---:|:---|:---|
|class...|Args||
#### Parameters
||||
|---:|:---|:---|
|const SizeType|capacity||
|Args &&...|args||
------
### `constexpr SizeType append(const StringView & str)`

#### Parameters
||||
|---:|:---|:---|
|const StringView &|str||
------
### `constexpr SizeType append(const T c)`

#### Parameters
||||
|---:|:---|:---|
|const T|c||
------
### `constexpr SizeType append(const T * data, const SizeType n)`

#### Parameters
||||
|---:|:---|:---|
|const T *|data||
|const SizeType|n||
------
### `constexpr SizeType append(const SizeType n, const T c)`

#### Parameters
||||
|---:|:---|:---|
|const SizeType|n||
|const T|c||
------
### `template<class U> constexpr SizeType assign(const U & data)`

#### Template
||||
|---:|:---|:---|
|class U|None||
#### Parameters
||||
|---:|:---|:---|
|const U &|data||
------
### `constexpr SizeType capacity() const`

------
### `constexpr void clear()`

------
### `constexpr void resize(const SizeType n)`

#### Parameters
||||
|---:|:---|:---|
|const SizeType|n||
------
### `template<class U> constexpr String< T, Impl > & operator+=(const U & data)`

#### Template
||||
|---:|:---|:---|
|class U|None||
#### Parameters
||||
|---:|:---|:---|
|const U &|data||
------
### `template<class U> constexpr String< T, Impl > & operator=(const U & data)`

#### Template
||||
|---:|:---|:---|
|class U|None||
#### Parameters
||||
|---:|:---|:---|
|const U &|data||
------
### `const SizeType capacity_`

