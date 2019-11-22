# [`bzd`](../../index.md)::[`assert`](../index.md)

## ` assert`

### Function
||||
|---:|:---|:---|
|constexpr void|[`isTrue(const bool condition, const bzd::StringView & fmt, Args &&... args)`](.)||
|constexpr void|[`isTrue(const bool condition)`](.)||
|constexpr void|[`isTrue(const bool condition, const bzd::StringView & message)`](.)||
------
### `template<class... Args> constexpr void isTrue(const bool condition, const bzd::StringView & fmt, Args &&... args)`

#### Template
||||
|---:|:---|:---|
|class...|Args||
#### Parameters
||||
|---:|:---|:---|
|const bool|condition||
|const bzd::StringView &|fmt||
|Args &&...|args||
------
### `constexpr void isTrue(const bool condition)`

#### Parameters
||||
|---:|:---|:---|
|const bool|condition||
------
### `constexpr void isTrue(const bool condition, const bzd::StringView & message)`

#### Parameters
||||
|---:|:---|:---|
|const bool|condition||
|const bzd::StringView &|message||
