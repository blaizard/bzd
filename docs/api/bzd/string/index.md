# [`bzd`](../../index.md)::[`String`](../index.md)

## `template<SizeType N> class String`

#### Template
||||
|---:|:---|:---|
|SizeType|N||

|Function||
|:---|:---|
|[`String()`](./index.md)||
|[`String(const bzd::StringView & str)`](./index.md)||
|[`String(const SizeType n, const char c)`](./index.md)||
|[`operator=(const T & data)`](./index.md)||
|[`operator=(const String< N > & data)`](./index.md)||
|[`append(const StringView & str)`](./index.md)||
|[`append(const T c)`](./index.md)||
|[`append(const T * data, const SizeType n)`](./index.md)||
|[`append(const SizeType n, const T c)`](./index.md)||
|[`assign(const U & data)`](./index.md)||
|[`capacity() const`](./index.md)||
|[`clear()`](./index.md)||
|[`resize(const SizeType n)`](./index.md)||
|[`operator+=(const U & data)`](./index.md)||
|[`operator=(const U & data)`](./index.md)||

|Variable||
|:---|:---|
|[`capacity_`](./index.md)||
------
### `constexpr String()`

------
### `constexpr String(const bzd::StringView & str)`

#### Parameters
||||
|---:|:---|:---|
|const bzd::StringView &|str||
------
### `constexpr String(const SizeType n, const char c)`

#### Parameters
||||
|---:|:---|:---|
|const SizeType|n||
|const char|c||
------
### `template<class T> constexpr String< N > & operator=(const T & data)`

#### Template
||||
|---:|:---|:---|
|class T|None||
#### Parameters
||||
|---:|:---|:---|
|const T &|data||
------
### `constexpr String< N > & operator=(const String< N > & data)`

#### Parameters
||||
|---:|:---|:---|
|const String< N > &|data||
------
### `constexpr SizeType append(const StringView & str)`
*From bzd::impl::String*


#### Parameters
||||
|---:|:---|:---|
|const StringView &|str||
------
### `constexpr SizeType append(const T c)`
*From bzd::impl::String*


#### Parameters
||||
|---:|:---|:---|
|const T|c||
------
### `constexpr SizeType append(const T * data, const SizeType n)`
*From bzd::impl::String*


#### Parameters
||||
|---:|:---|:---|
|const T *|data||
|const SizeType|n||
------
### `constexpr SizeType append(const SizeType n, const T c)`
*From bzd::impl::String*


#### Parameters
||||
|---:|:---|:---|
|const SizeType|n||
|const T|c||
------
### `template<class U> constexpr SizeType assign(const U & data)`
*From bzd::impl::String*


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
*From bzd::impl::String*


------
### `constexpr void clear()`
*From bzd::impl::String*


------
### `constexpr void resize(const SizeType n)`
*From bzd::impl::String*


#### Parameters
||||
|---:|:---|:---|
|const SizeType|n||
------
### `template<class U> constexpr String< T, Impl > & operator+=(const U & data)`
*From bzd::impl::String*


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
*From bzd::impl::String*


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
*From bzd::impl::String*


