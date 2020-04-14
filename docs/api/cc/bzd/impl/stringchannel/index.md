# [`bzd`](../../../index.md)::[`impl`](../../index.md)::[`StringChannel`](../index.md)

## `class StringChannel`


|Function||
|:---|:---|
|[`StringChannel(bzd::interface::String & str)`](./index.md)||
|[`connect()`](./index.md)||
|[`connect()`](./index.md)||
|[`disconnect()`](./index.md)||
|[`disconnect()`](./index.md)||
|[`str() const`](./index.md)||
|[`str()`](./index.md)||
|[`write(const bzd::StringView & data)`](./index.md)||
|[`write(const bzd::Span< const UInt8Type > & data)`](./index.md)||
|[`write(const Span< const char > & data)`](./index.md)||
|[`write(const Span< const T > & data)`](./index.md)||
|[`write(const T & data)`](./index.md)||
------
### `constexpr StringChannel(bzd::interface::String & str)`

#### Parameters
||||
|---:|:---|:---|
|bzd::interface::String &|str||
------
### `virtual bzd::Expected< void > connect()`
*From bzd::impl::OChannel*


------
### `virtual bzd::Expected< void > connect()`
*From bzd::impl::IOChannelCommon*


------
### `virtual bzd::Expected< void > disconnect()`
*From bzd::impl::OChannel*


------
### `virtual bzd::Expected< void > disconnect()`
*From bzd::impl::IOChannelCommon*


------
### `constexpr const bzd::interface::String & str() const`

------
### `constexpr bzd::interface::String & str()`

------
### `bzd::Expected< SizeType > write(const bzd::StringView & data)`

#### Parameters
||||
|---:|:---|:---|
|const bzd::StringView &|data||
------
### `bzd::Expected< SizeType > write(const bzd::Span< const UInt8Type > & data)`

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
