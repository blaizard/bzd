# [`bzd`](../../../index.md)::[`impl`](../../index.md)::[`Variant`](../index.md)

## `template<class StorageType, class... Ts> class Variant`

#### Template
||||
|---:|:---|:---|
|class|StorageType||
|class...|Ts||

|Function||
|:---|:---|
|[`Variant()`](./index.md)||
|[`Variant(T && value)`](./index.md)||
|[`get() const`](./index.md)||
|[`get()`](./index.md)||
|[`is() const`](./index.md)||
|[`match(Functors &&... funcs) const`](./index.md)||
------
### `constexpr Variant()`
Default constructor
------
### `template<class T, bzd::typeTraits::EnableIf< Contains< T >::value > *> constexpr Variant(T && value)`
Value constructor
#### Template
||||
|---:|:---|:---|
|class T|None||
|bzd::typeTraits::EnableIf< Contains< T >::value > *|None||
#### Parameters
||||
|---:|:---|:---|
|T &&|value||
------
### `template<class T> constexpr bzd::Expected< const T &, bool > get() const`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> constexpr bzd::Expected< T &, bool > get()`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> constexpr bool is() const`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class... Functors> constexpr void match(Functors &&... funcs) const`

#### Template
||||
|---:|:---|:---|
|class...|Functors||
#### Parameters
||||
|---:|:---|:---|
|Functors &&...|funcs||
