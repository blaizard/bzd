# [`bzd`](../../../index.md)::[`impl`](../../index.md)::[`TupleElem`](../index.md)

## `template<SizeType N, class T> class TupleElem`

#### Template
||||
|---:|:---|:---|
|SizeType|N||
|class T|None||

|Function||
|:---|:---|
|[`TupleElem()`](./index.md)||
|[`TupleElem(Value && value)`](./index.md)||
|[`TupleElem(const NoType &)`](./index.md)||
|[`get()`](./index.md)||
|[`get() const`](./index.md)||
------
### `constexpr TupleElem()`

------
### `template<class Value, typeTraits::EnableIf<!typeTraits::isSame< Value, NoType >> *> constexpr TupleElem(Value && value)`

#### Template
||||
|---:|:---|:---|
|class Value|None||
|typeTraits::EnableIf<!typeTraits::isSame< Value, NoType >> *|None||
#### Parameters
||||
|---:|:---|:---|
|Value &&|value||
------
### `constexpr TupleElem(const NoType &)`

#### Parameters
||||
|---:|:---|:---|
|const NoType &|None||
------
### `constexpr T & get()`

------
### `constexpr const T & get() const`

