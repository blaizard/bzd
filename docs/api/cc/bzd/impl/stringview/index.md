# [`bzd`](../../../index.md)::[`impl`](../../index.md)::[`StringView`](../index.md)

## `template<class T, class Impl> class StringView`

#### Template
||||
|---:|:---|:---|
|class T|None||
|class Impl|None||

|Function||
|:---|:---|
|[`StringView()`](./index.md)||
|[`StringView(const T *const str)`](./index.md)||
|[`StringView(const T *const str, const SizeType size)`](./index.md)||
|[`StringView(const Span< char > & span)`](./index.md)||
|[`clear()`](./index.md)||
|[`removePrefix(const SizeType n)`](./index.md)||
|[`subStr(const SizeType pos, const SizeType count) const`](./index.md)||
------
### `constexpr StringView()`

------
### `constexpr StringView(const T *const str)`

#### Parameters
||||
|---:|:---|:---|
|const T *const|str||
------
### `constexpr StringView(const T *const str, const SizeType size)`

#### Parameters
||||
|---:|:---|:---|
|const T *const|str||
|const SizeType|size||
------
### `constexpr StringView(const Span< char > & span)`

#### Parameters
||||
|---:|:---|:---|
|const Span< char > &|span||
------
### `constexpr void clear()`

------
### `constexpr void removePrefix(const SizeType n)`

#### Parameters
||||
|---:|:---|:---|
|const SizeType|n||
------
### `constexpr StringView subStr(const SizeType pos, const SizeType count) const`

#### Parameters
||||
|---:|:---|:---|
|const SizeType|pos||
|const SizeType|count||
