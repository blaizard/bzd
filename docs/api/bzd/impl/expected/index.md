# [`bzd`](../../../index.md)::[`impl`](../../index.md)::[`Expected`](../index.md)

## `template<class T, class E> class Expected`

#### Template
||||
|---:|:---|:---|
|class|T||
|class|E||

|Function||
|:---|:---|
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
### `constexpr Expected(T && value)`

#### Parameters
||||
|---:|:---|:---|
|T &&|value||
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

