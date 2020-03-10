# [`bzd`](../../index.md)::[`StringStream`](../index.md)

## `template<SizeType N> class StringStream`

#### Template
||||
|---:|:---|:---|
|SizeType|N||

|Function||
|:---|:---|
|[`StringStream()`](./index.md)||
|[`connect()`](./index.md)||
|[`disconnect()`](./index.md)||
|[`str() const`](./index.md)||
|[`str()`](./index.md)||
|[`write(const bzd::StringView & data)`](./index.md)||
|[`write(const bzd::Span< const UInt8Type > & data)`](./index.md)||
|[`write(const Span< const char > & data)`](./index.md)||
|[`write(const Span< const T > & data)`](./index.md)||
|[`write(const T & data)`](./index.md)||
------
### `constexpr StringStream()`

------
### `virtual bzd::Expected< void > connect()`
*From bzd::impl::IOChannelCommon*


------
### `virtual bzd::Expected< void > disconnect()`
*From bzd::impl::IOChannelCommon*


------
### `constexpr const bzd::interface::String & str() const`
*From bzd::impl::StringStream*


------
### `constexpr bzd::interface::String & str()`
*From bzd::impl::StringStream*


------
### `bzd::Expected< SizeType > write(const bzd::StringView & data)`
*From bzd::impl::StringStream*


#### Parameters
||||
|---:|:---|:---|
|const bzd::StringView &|data||
------
### `bzd::Expected< SizeType > write(const bzd::Span< const UInt8Type > & data)`
*From bzd::impl::StringStream*


#### Parameters
||||
|---:|:---|:---|
|const bzd::Span< const UInt8Type > &|data||
------
### `bzd::Expected< SizeType > write(const Span< const char > & data)`
*From bzd::impl::OChannel*


#### Parameters
||||
|---:|:---|:---|
|const Span< const char > &|data||
------
### `virtual bzd::Expected< SizeType > write(const Span< const T > & data)`
*From bzd::impl::OChannel*


#### Parameters
||||
|---:|:---|:---|
|const Span< const T > &|data||
------
### `virtual bzd::Expected< SizeType > write(const T & data)`
*From bzd::impl::OChannel*


#### Parameters
||||
|---:|:---|:---|
|const T &|data||
