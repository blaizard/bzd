# [`bzd`](../../../index.md)::[`impl`](../../index.md)::[`IChannel`](../index.md)

## `template<class T> class IChannel`

#### Template
||||
|---:|:---|:---|
|class T|None||

|Function||
|:---|:---|
|[`connect()`](./index.md)||
|[`disconnect()`](./index.md)||
|[`read(Span< T > & data)`](./index.md)||
|[`read(T & data)`](./index.md)||
------
### `virtual bzd::Expected< void > connect()`
*From bzd::impl::IOChannelCommon*


------
### `virtual bzd::Expected< void > disconnect()`
*From bzd::impl::IOChannelCommon*


------
### `virtual bzd::Expected< SizeType > read(Span< T > & data)`

#### Parameters
||||
|---:|:---|:---|
|Span< T > &|data||
------
### `virtual bzd::Expected< SizeType > read(T & data)`

#### Parameters
||||
|---:|:---|:---|
|T &|data||
