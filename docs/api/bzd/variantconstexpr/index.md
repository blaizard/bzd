# [`bzd`](../../index.md)::[`VariantConstexpr`](../index.md)

## `template<class... Ts> class VariantConstexpr`

#### Template
||||
|---:|:---|:---|
|class...|Ts||

|Function||
|:---|:---|
|[`VariantConstexpr(Args &&... args)`](./index.md)||
|[`get() const`](./index.md)||
|[`get()`](./index.md)||
|[`index() const`](./index.md)||
|[`is() const`](./index.md)||
|[`match(Functors &&... funcs) const`](./index.md)||
|[`Variant()`](./index.md)||
|[`Variant(T && value)`](./index.md)||
|[`Variant(T && value)`](./index.md)||
|[`Variant(const Variant< StorageType, Ts... > & variant)`](./index.md)||
------
### `template<class... Args> constexpr VariantConstexpr(Args &&... args)`

#### Template
||||
|---:|:---|:---|
|class...|Args||
#### Parameters
||||
|---:|:---|:---|
|Args &&...|args||
------
### `template<class T> constexpr bzd::Expected< const T &, bool > get() const`
*From bzd::impl::Variant*


#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> constexpr bzd::Expected< T &, bool > get()`
*From bzd::impl::Variant*


#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `constexpr bzd::SizeType index() const`
*From bzd::impl::Variant*


------
### `template<class T> constexpr bool is() const`
*From bzd::impl::Variant*


#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class... Functors> constexpr void match(Functors &&... funcs) const`
*From bzd::impl::Variant*


#### Template
||||
|---:|:---|:---|
|class...|Functors||
#### Parameters
||||
|---:|:---|:---|
|Functors &&...|funcs||
------
### `constexpr Variant()`
*From bzd::impl::Variant*

Default constructor
------
### `template<class T, int Index, bzd::typeTraits::EnableIf< Index !=-1 > *> constexpr Variant(T && value)`
*From bzd::impl::Variant*

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
*From bzd::impl::Variant*

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
*From bzd::impl::Variant*

Copy constructor
#### Parameters
||||
|---:|:---|:---|
|const Variant< StorageType, Ts... > &|variant||
