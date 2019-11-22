# [`bzd`](../../../index.md)::[`impl`](../../index.md)::[`Expected< void, E >`](../index.md)

## `template<class E> class Expected< void, E >`

#### Template
||||
|---:|:---|:---|
|class E|None||
### Function
||||
|---:|:---|:---|
|constexpr|[`Expected()`](.)||
|constexpr|[`Expected(U && value)`](.)||
|constexpr|[`Expected(impl::Unexpected< U > && u)`](.)||
|constexpr|[`Expected(Expected< T, E > && e)`](.)||
||[`~Expected()`](.)||
|constexpr|[`operator bool() const`](.)||
|constexpr const E &|[`error() const`](.)||
|constexpr const Value &|[`operator*() const`](.)||
|constexpr Value &|[`operator*()`](.)||
|constexpr const Value *|[`operator->() const`](.)||
|constexpr Value *|[`operator->()`](.)||
### Variable
||||
|---:|:---|:---|
|ValueContainer|[`value_`](.)||
|E|[`error_`](.)||
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


