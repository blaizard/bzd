# [`bzd`](../../index.md)::[`Map`](../index.md)

## `template<class K, class V, SizeType N> class Map`

#### Template
||||
|---:|:---|:---|
|class|K||
|class|V||
|SizeType|N||

|Struct||
|:---|:---|
|[`Element`](./index.md)||

|Function||
|:---|:---|
|[`Map()`](./index.md)||
|[`find(const K & key) const`](./index.md)||
|[`insert(const K & key, V && value)`](./index.md)||
|[`insert(const Iterator & it, V && value)`](./index.md)||
|[`operator[](const K & key) const`](./index.md)||

|Typedef||
|:---|:---|
|[`Iterator`](./index.md)|alias of [`bzd::impl::Vector`](../impl/vector/index.md)|
------
### `struct Element`
*From bzd::impl::Map*


------
### `constexpr Map()`

------
### `constexpr bzd::Optional< Iterator > find(const K & key) const`
*From bzd::impl::Map*

Search for a specific element in the map.
#### Parameters
||||
|---:|:---|:---|
|const K &|key||
------
### `constexpr void insert(const K & key, V && value)`
*From bzd::impl::Map*

Insert a new element or replace the existing one
#### Parameters
||||
|---:|:---|:---|
|const K &|key||
|V &&|value||
------
### `constexpr void insert(const Iterator & it, V && value)`
*From bzd::impl::Map*


#### Parameters
||||
|---:|:---|:---|
|const Iterator &|it||
|V &&|value||
------
### `constexpr V & operator[](const K & key) const`
*From bzd::impl::Map*


#### Parameters
||||
|---:|:---|:---|
|const K &|key||
------
### `typedef Iterator`
*From bzd::impl::Map*


