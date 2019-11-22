# [`bzd`](../../../index.md)::[`impl`](../../index.md)::[`StringView`](../index.md)

## `template<class T, class Impl> class StringView`

#### Template
||||
|---:|:---|:---|
|class T|None||
|class Impl|None||
### Function
||||
|---:|:---|:---|
|constexpr|[`StringView(const T *const str)`](.)||
|constexpr|[`StringView(const T *const str, const SizeType size)`](.)||
|constexpr|[`StringView(const Span< char > & span)`](.)||
|constexpr StringView|[`subStr(const SizeType pos, const SizeType count) const`](.)||
|constexpr void|[`removePrefix(const SizeType n)`](.)||
|constexpr void|[`clear()`](.)||
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
### `constexpr StringView subStr(const SizeType pos, const SizeType count) const`

#### Parameters
||||
|---:|:---|:---|
|const SizeType|pos||
|const SizeType|count||
------
### `constexpr void removePrefix(const SizeType n)`

#### Parameters
||||
|---:|:---|:---|
|const SizeType|n||
------
### `constexpr void clear()`

