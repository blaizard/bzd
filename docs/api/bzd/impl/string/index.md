# [`bzd`](../../../index.md)::[`impl`](../../index.md)::[`String`](../index.md)

## `template<class T, class Impl> class String`

#### Template
||||
|---:|:---|:---|
|class T|None||
|class Impl|None||

|Function||
|:---|:---|
|[`String(const SizeType capacity, Args &&... args)`](./index.md)||
|[`append(const StringView & str)`](./index.md)||
|[`append(const T c)`](./index.md)||
|[`append(const T * data, const SizeType n)`](./index.md)||
|[`append(const SizeType n, const T c)`](./index.md)||
|[`assign(const U & data)`](./index.md)||
|[`capacity() const`](./index.md)||
|[`clear()`](./index.md)||
|[`operator+=(const U & data)`](./index.md)||
|[`operator=(const U & data)`](./index.md)||
|[`resize(const SizeType n)`](./index.md)||

|Variable||
|:---|:---|
|[`capacity_`](./index.md)||
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
### `constexpr void resize(const SizeType n)`

#### Parameters
||||
|---:|:---|:---|
|const SizeType|n||
------
### `const SizeType capacity_`

