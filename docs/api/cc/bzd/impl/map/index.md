# [`bzd`](../../../index.md)::[`impl`](../../index.md)::[`Map`](../index.md)

## `template<class K, class V> class Map`
Flat map implementation.
#### Template
||||
|---:|:---|:---|
|class K|None||
|class V|None||

|Struct||
|:---|:---|
|[`Element`](element/index.md)||

|Function||
|:---|:---|
|[`Map(bzd::interface::Vector< Element > & data)`](./index.md)||
|[`contains(const K & key) const`](./index.md)||
|[`find(const K & key) const`](./index.md)||
|[`insert(const K & key, V && value)`](./index.md)||
|[`insert(const Iterator & it, V && value)`](./index.md)||
|[`operator[](const K & key) const`](./index.md)||

|Typedef||
|:---|:---|
|[`Iterator`](./index.md)|alias of [`bzd::impl::Vector`](../vector/index.md)|
------
### `struct Element`

------
### `explicit constexpr Map(bzd::interface::Vector< Element > & data)`

#### Parameters
||||
|---:|:---|:---|
|bzd::interface::Vector< Element > &|data||
------
### `constexpr bool contains(const K & key) const`

#### Parameters
||||
|---:|:---|:---|
|const K &|key||
------
### `constexpr bzd::Optional< Iterator > find(const K & key) const`
Search for a specific element in the map.
#### Parameters
||||
|---:|:---|:---|
|const K &|key||
------
### `constexpr void insert(const K & key, V && value)`
Insert a new element or replace the existing one
#### Parameters
||||
|---:|:---|:---|
|const K &|key||
|V &&|value||
------
### `constexpr void insert(const Iterator & it, V && value)`

#### Parameters
||||
|---:|:---|:---|
|const Iterator &|it||
|V &&|value||
------
### `constexpr V & operator[](const K & key) const`

#### Parameters
||||
|---:|:---|:---|
|const K &|key||
------
### `typedef Iterator`

