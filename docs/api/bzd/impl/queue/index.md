# [`bzd`](../../../index.md)::[`impl`](../../index.md)::[`Queue`](../index.md)

## `template<class T, class CapacityType> class Queue`

#### Template
||||
|---:|:---|:---|
|class|T||
|class|CapacityType||

|Function||
|:---|:---|
|[`Queue(const bzd::Span< Element > data)`](./index.md)||
|[`capacity() const`](./index.md)||
|[`empty() const`](./index.md)||
|[`front()`](./index.md)||
|[`front() const`](./index.md)||
|[`back()`](./index.md)||
|[`back() const`](./index.md)||
|[`push(T && value)`](./index.md)||
|[`pop()`](./index.md)||
------
### `explicit constexpr Queue(const bzd::Span< Element > data)`

#### Parameters
||||
|---:|:---|:---|
|const bzd::Span< Element >|data||
------
### `constexpr CapacityType capacity() const`

------
### `constexpr bool empty() const`

------
### `constexpr T & front()`

------
### `constexpr const T & front() const`

------
### `constexpr T & back()`

------
### `constexpr const T & back() const`

------
### `constexpr void push(T && value)`

#### Parameters
||||
|---:|:---|:---|
|T &&|value||
------
### `constexpr void pop()`

