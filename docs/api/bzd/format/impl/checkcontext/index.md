# [`bzd`](../../../../index.md)::[`format`](../../../index.md)::[`impl`](../../index.md)::[`CheckContext`](../index.md)

## `class CheckContext`


|Class||
|:---|:---|
|[`Iterator`](./index.md)||

|Function||
|:---|:---|
|[`CheckContext()`](./index.md)||
|[`addSubstring(const bzd::StringView &)`](./index.md)||
|[`addMetadata(const Metadata & metadata)`](./index.md)||
|[`onError(const bzd::StringView & message) const`](./index.md)||
|[`ConstexprVector()`](./index.md)||
|[`ConstexprVector(Args &&... args)`](./index.md)||
|[`begin() const`](./index.md)||
|[`end() const`](./index.md)||
|[`size() const`](./index.md)||
|[`capacity() const`](./index.md)||
|[`push_back(const T & element)`](./index.md)||
|[`operator[](const SizeType index)`](./index.md)||
|[`operator[](const SizeType index) const`](./index.md)||
------
### `class Iterator`
*From bzd::format::impl::ConstexprVector*


------
### `constexpr CheckContext()`

------
### `constexpr void addSubstring(const bzd::StringView &)`

#### Parameters
||||
|---:|:---|:---|
|const bzd::StringView &|None||
------
### `constexpr void addMetadata(const Metadata & metadata)`

#### Parameters
||||
|---:|:---|:---|
|const Metadata &|metadata||
------
### `void onError(const bzd::StringView & message) const`

#### Parameters
||||
|---:|:---|:---|
|const bzd::StringView &|message||
------
### `constexpr ConstexprVector()`
*From bzd::format::impl::ConstexprVector*


------
### `template<class... Args> constexpr ConstexprVector(Args &&... args)`
*From bzd::format::impl::ConstexprVector*


#### Template
||||
|---:|:---|:---|
|class...|Args||
#### Parameters
||||
|---:|:---|:---|
|Args &&...|args||
------
### `constexpr Iterator begin() const`
*From bzd::format::impl::ConstexprVector*


------
### `constexpr Iterator end() const`
*From bzd::format::impl::ConstexprVector*


------
### `constexpr SizeType size() const`
*From bzd::format::impl::ConstexprVector*


------
### `constexpr SizeType capacity() const`
*From bzd::format::impl::ConstexprVector*


------
### `constexpr void push_back(const T & element)`
*From bzd::format::impl::ConstexprVector*


#### Parameters
||||
|---:|:---|:---|
|const T &|element||
------
### `constexpr DataType & operator[](const SizeType index)`
*From bzd::format::impl::ConstexprVector*


#### Parameters
||||
|---:|:---|:---|
|const SizeType|index||
------
### `constexpr const DataType & operator[](const SizeType index) const`
*From bzd::format::impl::ConstexprVector*


#### Parameters
||||
|---:|:---|:---|
|const SizeType|index||
