# [`bzd`](../../index.md)::[`StringStream`](../index.md)

## `template<SizeType N> class StringStream`

#### Template
||||
|---:|:---|:---|
|SizeType|N||

|Function||
|:---|:---|
|[`StringStream()`](./index.md)||
|[`str() const`](./index.md)||
|[`str()`](./index.md)||
|[`write(const bzd::StringView & data)`](./index.md)||
|[`write(const bzd::Span< const char > & data)`](./index.md)||
|[`write(const Span< const T > & data)`](./index.md)||
------
### `constexpr StringStream()`

------
### `constexpr const bzd::interface::String & str() const`
*From bzd::impl::StringStream*


------
### `constexpr bzd::interface::String & str()`
*From bzd::impl::StringStream*


------
### `SizeType write(const bzd::StringView & data)`
*From bzd::impl::StringStream*


#### Parameters
||||
|---:|:---|:---|
|const bzd::StringView &|data||
------
### `SizeType write(const bzd::Span< const char > & data)`
*From bzd::impl::StringStream*


#### Parameters
||||
|---:|:---|:---|
|const bzd::Span< const char > &|data||
------
### `virtual SizeType write(const Span< const T > & data)`
*From bzd::impl::OStream*


#### Parameters
||||
|---:|:---|:---|
|const Span< const T > &|data||
