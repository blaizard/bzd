# [`bzd`](../../../index.md)::[`impl`](../../index.md)::[`Queue`](../index.md)

## `template<class T, class CapacityType> class Queue`

#### Template
||||
|---:|:---|:---|
|class T|None||
|class CapacityType|None||

|Function||
|:---|:---|
|[`Queue(const bzd::Span< Element > data)`](./index.md)||
|[`back()`](./index.md)||
|[`back() const`](./index.md)||
|[`capacity() const`](./index.md)||
|[`empty() const`](./index.md)||
|[`front()`](./index.md)||
|[`front() const`](./index.md)||
|[`pop()`](./index.md)||
|[`push(T && value)`](./index.md)||
------
### `explicit constexpr Queue(const bzd::Span< Element > data)`

#### Parameters
||||
|---:|:---|:---|
|const bzd::Span< Element >|data||
------
### `constexpr T & back()`

------
### `constexpr const T & back() const`

------
### `constexpr CapacityType capacity() const`

------
### `constexpr bool empty() const`

------
### `constexpr T & front()`

------
### `constexpr const T & front() const`

------
### `constexpr void pop()`

------
### `constexpr void push(T && value)`

#### Parameters
||||
|---:|:---|:---|
|T &&|value||
