# [`bzd`](../../../index.md)::[`impl`](../../index.md)::[`Variant`](../index.md)

## `template<class StorageType, class... Ts> class Variant`

#### Template
||||
|---:|:---|:---|
|class|StorageType||
|class...|Ts||
### Function
||||
|---:|:---|:---|
|constexpr|[`Variant()`](.)||
|constexpr|[`Variant(T && value)`](.)||
|constexpr bool|[`is() const`](.)||
|constexpr bzd::Expected< const T &, bool >|[`get() const`](.)||
|constexpr bzd::Expected< T &, bool >|[`get()`](.)||
|constexpr void|[`match(Functors &&... funcs) const`](.)||
------
### `constexpr Variant()`
Default constructor
------
### `template<class T, typename bzd::typeTraits::enableIf< Contains< T >::value >::type *> constexpr Variant(T && value)`
Value constructor
#### Template
||||
|---:|:---|:---|
|class T|None||
|typename bzd::typeTraits::enableIf< Contains< T >::value >::type *|None||
#### Parameters
||||
|---:|:---|:---|
|T &&|value||
------
### `template<class T> constexpr bool is() const`

#### Template
||||
|---:|:---|:---|
|class T|None||
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
### `template<class... Functors> constexpr void match(Functors &&... funcs) const`

#### Template
||||
|---:|:---|:---|
|class...|Functors||
#### Parameters
||||
|---:|:---|:---|
|Functors &&...|funcs||
