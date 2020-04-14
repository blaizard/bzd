# [`bzd`](../../../index.md)::[`impl`](../../index.md)::[`Optional`](../index.md)

## `template<class T> class Optional`

#### Template
||||
|---:|:---|:---|
|class T|None||

|Function||
|:---|:---|
|[`Optional(T && value)`](./index.md)||
|[`Optional()`](./index.md)||
|[`operator bool() const`](./index.md)||
|[`operator*() const`](./index.md)||
|[`operator*()`](./index.md)||
|[`operator->() const`](./index.md)||
|[`operator->()`](./index.md)||
|[`valueOr(const Value & defaultValue) const`](./index.md)||

|Variable||
|:---|:---|
|[`empty_`](./index.md)||
|[`value_`](./index.md)||
------
### `constexpr Optional(T && value)`

#### Parameters
||||
|---:|:---|:---|
|T &&|value||
------
### `constexpr Optional()`

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
### `constexpr const Value & valueOr(const Value & defaultValue) const`

#### Parameters
||||
|---:|:---|:---|
|const Value &|defaultValue||
------
### `bzd::UInt8Type empty_`

------
### `ValueContainer value_`

