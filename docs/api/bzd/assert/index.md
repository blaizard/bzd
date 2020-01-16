# [`bzd`](../../index.md)::[`assert`](../index.md)

## ` assert`


|Namespace||
|:---|:---|
|[`impl`](impl/index.md)||

|Function||
|:---|:---|
|[`isTrue(const bool condition, Args &&... args)`](./index.md)||
|[`isTrue(const bool condition)`](./index.md)||
|[`isTrue(const bool condition, const bzd::StringView & message)`](./index.md)||
|[`isTrueConstexpr(const bool condition)`](./index.md)||
|[`unreachable()`](./index.md)||
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
### `void isTrue(const bool condition)`

#### Parameters
||||
|---:|:---|:---|
|const bool|condition||
------
### `void isTrue(const bool condition, const bzd::StringView & message)`

#### Parameters
||||
|---:|:---|:---|
|const bool|condition||
|const bzd::StringView &|message||
------
### `constexpr bool isTrueConstexpr(const bool condition)`

#### Parameters
||||
|---:|:---|:---|
|const bool|condition||
------
### `void unreachable()`

