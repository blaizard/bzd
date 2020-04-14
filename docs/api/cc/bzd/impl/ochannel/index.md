# [`bzd`](../../../index.md)::[`impl`](../../index.md)::[`OChannel`](../index.md)

## `template<class T> class OChannel`

#### Template
||||
|---:|:---|:---|
|class T|None||

|Function||
|:---|:---|
|[`connect()`](./index.md)||
|[`disconnect()`](./index.md)||
|[`write(const Span< const char > & data)`](./index.md)||
|[`write(const Span< const T > & data)`](./index.md)||
|[`write(const T & data)`](./index.md)||
------
### `virtual bzd::Expected< void > connect()`
*From bzd::impl::IOChannelCommon*


------
### `virtual bzd::Expected< void > disconnect()`
*From bzd::impl::IOChannelCommon*


------
### `bzd::Expected< SizeType > write(const Span< const char > & data)`

#### Parameters
||||
|---:|:---|:---|
|const Span< const char > &|data||
------
### `virtual bzd::Expected< SizeType > write(const Span< const T > & data)`

#### Parameters
||||
|---:|:---|:---|
|const Span< const T > &|data||
------
### `virtual bzd::Expected< SizeType > write(const T & data)`

#### Parameters
||||
|---:|:---|:---|
|const T &|data||
