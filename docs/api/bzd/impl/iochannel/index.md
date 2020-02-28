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
|[`write(const Span< const T > & data)`](./index.md)||
|[`write(const T & data)`](./index.md)||
------
### `virtual void connect()`
*From bzd::impl::IChannel*


------
### `virtual void connect()`
*From bzd::impl::IOChannelCommon*


------
### `virtual void connect()`
*From bzd::impl::IOChannelCommon*


------
### `virtual void disconnect()`
*From bzd::impl::IChannel*


------
### `virtual void disconnect()`
*From bzd::impl::IOChannelCommon*


------
### `virtual void disconnect()`
*From bzd::impl::IOChannelCommon*


------
### `virtual SizeType read(Span< T > & data)`
*From bzd::impl::IChannel*


#### Parameters
||||
|---:|:---|:---|
|Span< T > &|data||
------
### `virtual SizeType read(T & data)`
*From bzd::impl::IChannel*


#### Parameters
||||
|---:|:---|:---|
|T &|data||
------
### `virtual SizeType write(const Span< const T > & data)`
*From bzd::impl::OChannel*


#### Parameters
||||
|---:|:---|:---|
|const Span< const T > &|data||
------
### `virtual SizeType write(const T & data)`
*From bzd::impl::OChannel*


#### Parameters
||||
|---:|:---|:---|
|const T &|data||
