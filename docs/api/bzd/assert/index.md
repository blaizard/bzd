# [`bzd`](../../index.md)::[`assert`](../index.md)

## ` assert`


|Namespace||
|:---|:---|
|[`impl`](impl/index.md)||

|Function||
|:---|:---|
|[`isExpected(const Expected< T, E > & expected)`](./index.md)||
|[`isTrue(const bool condition, Args &&... args)`](./index.md)||
|[`isTrue(const bool condition)`](./index.md)||
|[`isTrue(const bool condition, const char * message)`](./index.md)||
|[`isTrueConstexpr(const bool condition)`](./index.md)||
|[`unreachable()`](./index.md)||
------
### `template<class T, class E> constexpr void isExpected(const Expected< T, E > & expected)`

#### Template
||||
|---:|:---|:---|
|class T|None||
|class E|None||
#### Parameters
||||
|---:|:---|:---|
|const Expected< T, E > &|expected||
------
### `template<class... Args> constexpr void isTrue(const bool condition, Args &&... args)`

#### Template
||||
|---:|:---|:---|
|class...|Args||
#### Parameters
||||
|---:|:---|:---|
|const bool|condition||
|Args &&...|args||
------
### `constexpr void isTrue(const bool condition)`

#### Parameters
||||
|---:|:---|:---|
|const bool|condition||
------
### `constexpr void isTrue(const bool condition, const char * message)`

#### Parameters
||||
|---:|:---|:---|
|const bool|condition||
|const char *|message||
------
### `constexpr bool isTrueConstexpr(const bool condition)`

#### Parameters
||||
|---:|:---|:---|
|const bool|condition||
------
### `void unreachable()`

