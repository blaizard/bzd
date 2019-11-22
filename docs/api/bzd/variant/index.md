# [`bzd`](../../index.md)::[`Variant`](../index.md)

## `template<class... Ts> class Variant`

#### Template
||||
|---:|:---|:---|
|class...|Ts||
### Function
||||
|---:|:---|:---|
|constexpr|[`Variant(Args &&... args)`](.)||
||[`~Variant()`](.)||
|constexpr void|[`emplace(Args &&... args)`](.)||
|constexpr bool|[`is() const`](.)||
|constexpr bzd::Expected< const T &, bool >|[`get() const`](.)||
|constexpr bzd::Expected< T &, bool >|[`get()`](.)||
|constexpr void|[`match(Functors &&... funcs) const`](.)||
------
### `template<class... Args> constexpr Variant(Args &&... args)`

#### Template
||||
|---:|:---|:---|
|class...|Args||
#### Parameters
||||
|---:|:---|:---|
|Args &&...|args||
------
### ` ~Variant()`

------
### `template<class T, class... Args, typename bzd::typeTraits::enableIf< Contains< T >::value >::type *> constexpr void emplace(Args &&... args)`

#### Template
||||
|---:|:---|:---|
|class T|None||
|class...|Args||
|typename bzd::typeTraits::enableIf< Contains< T >::value >::type *|None||
#### Parameters
||||
|---:|:---|:---|
|Args &&...|args||
------
### `template<class T> constexpr bool is() const`
*From bzd::impl::Variant*


#### Template
||||
|---:|:---|:---|
|class T|None||
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
