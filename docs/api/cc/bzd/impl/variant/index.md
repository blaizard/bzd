# [`bzd`](../../../index.md)::[`impl`](../../index.md)::[`Variant`](../index.md)

## `template<class StorageType, class... Ts> class Variant`

#### Template
||||
|---:|:---|:---|
|class StorageType|None||
|class...|Ts||

|Function||
|:---|:---|
|[`Variant()`](./index.md)||
|[`Variant(T && value)`](./index.md)||
|[`Variant(T && value)`](./index.md)||
|[`Variant(const Variant< StorageType, Ts... > & variant)`](./index.md)||
|[`get() const`](./index.md)||
|[`get()`](./index.md)||
|[`index() const`](./index.md)||
|[`is() const`](./index.md)||
|[`match(Functors &&... funcs) const`](./index.md)||
------
### `constexpr Variant()`
Default constructor
------
### `template<class T, int Index, bzd::typeTraits::EnableIf< Index !=-1 > *> constexpr Variant(T && value)`
Value constructor (exact type match)
#### Template
||||
|---:|:---|:---|
|class T|None||
|int|Index||
|bzd::typeTraits::EnableIf< Index !=-1 > *|None||
#### Parameters
||||
|---:|:---|:---|
|T &&|value||
------
### `template<class T, int Index, bzd::typeTraits::EnableIf< Find< T >::value==-1 &&Index !=-1 > *> constexpr Variant(T && value)`
Value constructor (lazy, if constructible)
#### Template
||||
|---:|:---|:---|
|class T|None||
|int|Index||
|bzd::typeTraits::EnableIf< Find< T >::value==-1 &&Index !=-1 > *|None||
#### Parameters
||||
|---:|:---|:---|
|T &&|value||
------
### `constexpr Variant(const Variant< StorageType, Ts... > & variant)`
Copy constructor
#### Parameters
||||
|---:|:---|:---|
|const Variant< StorageType, Ts... > &|variant||
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
### `constexpr bzd::SizeType index() const`

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
