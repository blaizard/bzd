# [`bzd`](../../../index.md)::[`impl`](../../index.md)::[`StringStream`](../index.md)

## `class StringStream`


|Function||
|:---|:---|
|[`StringStream(bzd::interface::String & str)`](./index.md)||
|[`write(const bzd::StringView & data)`](./index.md)||
|[`write(const bzd::Span< const char > & data)`](./index.md)||
|[`str() const`](./index.md)||
|[`str()`](./index.md)||
|[`write(const Span< const T > & data)`](./index.md)||
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
