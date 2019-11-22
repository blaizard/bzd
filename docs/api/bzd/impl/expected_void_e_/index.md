# [`bzd`](../../../index.md)::[`impl`](../../index.md)::[`Expected< void, E >`](../index.md)

## `template<class E> class Expected< void, E >`

#### Template
||||
|---:|:---|:---|
|class E|None||
### Function
||||
|---:|:---|:---|
|constexpr|[`Expected()`](./index.md)||
|constexpr|[`Expected(U && value)`](./index.md)||
|constexpr|[`Expected(impl::Unexpected< U > && u)`](./index.md)||
|constexpr|[`Expected(Expected< T, E > && e)`](./index.md)||
||[`~Expected()`](./index.md)||
|constexpr|[`operator bool() const`](./index.md)||
|constexpr const E &|[`error() const`](./index.md)||
|constexpr const Value &|[`operator*() const`](./index.md)||
|constexpr Value &|[`operator*()`](./index.md)||
|constexpr const Value *|[`operator->() const`](./index.md)||
|constexpr Value *|[`operator->()`](./index.md)||
### Variable
||||
|---:|:---|:---|
|ValueContainer|[`value_`](./index.md)||
|E|[`error_`](./index.md)||
------
### `constexpr Expected()`

------
### `template<class U> constexpr Expected(U && value)`
*From bzd::impl::Expected*


#### Template
||||
|---:|:---|:---|
|class U|None||
#### Parameters
||||
|---:|:---|:---|
|U &&|value||
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


