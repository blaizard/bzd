# [`bzd`](../../index.md)::[`Variant`](../index.md)

## `template<class... Ts> class Variant`

#### Template
||||
|---:|:---|:---|
|class...|Ts||

|Function||
|:---|:---|
|[`Variant(Args &&... args)`](./index.md)||
|[`~Variant()`](./index.md)||
|[`emplace(Args &&... args)`](./index.md)||
|[`get() const`](./index.md)||
|[`get()`](./index.md)||
|[`index() const`](./index.md)||
|[`is() const`](./index.md)||
|[`match(Functors &&... funcs) const`](./index.md)||
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
### `template<class T, class... Args, bzd::typeTraits::EnableIf< Contains< T >::value > *> constexpr void emplace(Args &&... args)`

#### Template
||||
|---:|:---|:---|
|class T|None||
|class...|Args||
|bzd::typeTraits::EnableIf< Contains< T >::value > *|None||
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
