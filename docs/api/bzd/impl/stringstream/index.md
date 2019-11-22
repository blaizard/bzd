# [`bzd`](../../../index.md)::[`impl`](../../index.md)::[`StringStream`](../index.md)

## `class StringStream`

### Function
||||
|---:|:---|:---|
|constexpr|[`StringStream(bzd::interface::String & str)`](./index.md)||
|SizeType|[`write(const bzd::StringView & data)`](./index.md)||
|SizeType|[`write(const bzd::Span< const char > & data)`](./index.md)||
|constexpr const bzd::interface::String &|[`str() const`](./index.md)||
|constexpr bzd::interface::String &|[`str()`](./index.md)||
|SizeType|[`write(const Span< const T > & data)`](./index.md)||
------
### `constexpr StringStream(bzd::interface::String & str)`

#### Parameters
||||
|---:|:---|:---|
|bzd::interface::String &|str||
------
### `SizeType write(const bzd::StringView & data)`

#### Parameters
||||
|---:|:---|:---|
|const bzd::StringView &|data||
------
### `SizeType write(const bzd::Span< const char > & data)`

#### Parameters
||||
|---:|:---|:---|
|const bzd::Span< const char > &|data||
------
### `constexpr const bzd::interface::String & str() const`

------
### `constexpr bzd::interface::String & str()`

------
### `virtual SizeType write(const Span< const T > & data)`
*From bzd::impl::OStream*


#### Parameters
||||
|---:|:---|:---|
|const Span< const T > &|data||
