# [`bzd`](../../../index.md)::[`impl`](../../index.md)::[`Expected`](../index.md)

## `template<class T, class E> class Expected`

#### Template
||||
|---:|:---|:---|
|class|T||
|class|E||
### Function
||||
|---:|:---|:---|
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
### `template<class U> constexpr Expected(U && value)`

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

#### Parameters
||||
|---:|:---|:---|
|Expected< T, E > &&|e||
------
### ` ~Expected()`

------
### `constexpr operator bool() const`

------
### `constexpr const E & error() const`

------
### `constexpr const Value & operator*() const`

------
### `constexpr Value & operator*()`

------
### `constexpr const Value * operator->() const`

------
### `constexpr Value * operator->()`

------
### `ValueContainer value_`

------
### `E error_`

