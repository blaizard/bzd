# [`bzd`](../../../index.md)::[`impl`](../../index.md)::[`Expected< void, E >`](../index.md)

## `template<class E> class Expected< void, E >`

#### Template
||||
|---:|:---|:---|
|class E|None||

|Function||
|:---|:---|
|[`Expected()`](./index.md)||
|[`Expected(T && value)`](./index.md)||
|[`Expected(impl::Unexpected< U > && u)`](./index.md)||
|[`Expected(Expected< T, E > && e)`](./index.md)||
|[`~Expected()`](./index.md)||
|[`operator bool() const`](./index.md)||
|[`error() const`](./index.md)||
|[`operator*() const`](./index.md)||
|[`operator*()`](./index.md)||
|[`operator->() const`](./index.md)||
|[`operator->()`](./index.md)||

|Variable||
|:---|:---|
|[`value_`](./index.md)||
|[`error_`](./index.md)||
------
### `constexpr Expected()`

------
### `constexpr Expected(T && value)`
*From bzd::impl::Expected*


#### Parameters
||||
|---:|:---|:---|
|T &&|value||
------
### `template<class U> constexpr Expected(impl::Unexpected< U > && u)`
*From bzd::impl::Expected*


#### Template
||||
|---:|:---|:---|
|class U|None||
#### Parameters
||||
|---:|:---|:---|
|impl::Unexpected< U > &&|u||
------
### `constexpr Expected(Expected< T, E > && e)`
*From bzd::impl::Expected*


#### Parameters
||||
|---:|:---|:---|
|Expected< T, E > &&|e||
------
### ` ~Expected()`
*From bzd::impl::Expected*


------
### `constexpr operator bool() const`
*From bzd::impl::Expected*


------
### `constexpr const E & error() const`
*From bzd::impl::Expected*


------
### `constexpr const Value & operator*() const`
*From bzd::impl::Expected*


------
### `constexpr Value & operator*()`
*From bzd::impl::Expected*


------
### `constexpr const Value * operator->() const`
*From bzd::impl::Expected*


------
### `constexpr Value * operator->()`
*From bzd::impl::Expected*


------
### `ValueContainer value_`
*From bzd::impl::Expected*


------
### `E error_`
*From bzd::impl::Expected*


