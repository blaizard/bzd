# [`bzd`](../../../index.md)::[`impl`](../../index.md)::[`Expected`](../index.md)

## `template<class T, class E> class Expected`

#### Template
||||
|---:|:---|:---|
|class T|None||
|class E|None||

|Function||
|:---|:---|
|[`Expected(T && value)`](./index.md)||
|[`Expected(impl::Unexpected< U > && u)`](./index.md)||
|[`Expected(Expected< T, E > && e)`](./index.md)||
|[`~Expected()`](./index.md)||
|[`error() const`](./index.md)||
|[`operator bool() const`](./index.md)||
|[`operator*() const`](./index.md)||
|[`operator*()`](./index.md)||
|[`operator->() const`](./index.md)||
|[`operator->()`](./index.md)||

|Variable||
|:---|:---|
|[`error_`](./index.md)||
|[`value_`](./index.md)||
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
### `constexpr const E & error() const`

------
### `constexpr operator bool() const`

------
### `constexpr const Value & operator*() const`

------
### `constexpr Value & operator*()`

------
### `constexpr const Value * operator->() const`

------
### `constexpr Value * operator->()`

------
### `E error_`

------
### `ValueContainer value_`

