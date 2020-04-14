# [`bzd`](../../../index.md)::[`impl`](../../index.md)::[`IOChannel`](../index.md)

## `template<class T> class IOChannel`

#### Template
||||
|---:|:---|:---|
|class T|None||

|Function||
|:---|:---|
|[`connect()`](./index.md)||
|[`connect()`](./index.md)||
|[`connect()`](./index.md)||
|[`disconnect()`](./index.md)||
|[`disconnect()`](./index.md)||
|[`disconnect()`](./index.md)||
|[`read(Span< T > & data)`](./index.md)||
|[`read(T & data)`](./index.md)||
|[`write(const Span< const char > & data)`](./index.md)||
|[`write(const Span< const T > & data)`](./index.md)||
|[`write(const T & data)`](./index.md)||
------
### `virtual bzd::Expected< void > connect()`
*From bzd::impl::IChannel*


------
### `virtual bzd::Expected< void > connect()`
*From bzd::impl::IOChannelCommon*


------
### `virtual bzd::Expected< void > connect()`
*From bzd::impl::IOChannelCommon*


------
### `virtual bzd::Expected< void > disconnect()`
*From bzd::impl::IChannel*


------
### `virtual bzd::Expected< void > disconnect()`
*From bzd::impl::IOChannelCommon*


------
### `virtual bzd::Expected< void > disconnect()`
*From bzd::impl::IOChannelCommon*


------
### `virtual bzd::Expected< SizeType > read(Span< T > & data)`
*From bzd::impl::IChannel*


#### Parameters
||||
|---:|:---|:---|
|Span< T > &|data||
------
### `virtual bzd::Expected< SizeType > read(T & data)`
*From bzd::impl::IChannel*


#### Parameters
||||
|---:|:---|:---|
|T &|data||
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
