# [`bzd`](../../../../index.md)::[`format`](../../../index.md)::[`impl`](../../index.md)::[`PrintContext`](../index.md)

## `class PrintContext`


|Function||
|:---|:---|
|[`PrintContext(bzd::OChannel & stream, const bzd::interface::Vector< const bzd::format::impl::Formatter * > & args)`](./index.md)||
|[`addMetadata(const Metadata & metadata)`](./index.md)||
|[`addSubstring(const bzd::StringView & str)`](./index.md)||
|[`onError(const bzd::StringView & message) const`](./index.md)||
------
### `constexpr PrintContext(bzd::OChannel & stream, const bzd::interface::Vector< const bzd::format::impl::Formatter * > & args)`

#### Parameters
||||
|---:|:---|:---|
|bzd::OChannel &|stream||
|const bzd::interface::Vector< const bzd::format::impl::Formatter * > &|args||
------
### `void addMetadata(const Metadata & metadata)`

#### Parameters
||||
|---:|:---|:---|
|const Metadata &|metadata||
------
### `void addSubstring(const bzd::StringView & str)`

#### Parameters
||||
|---:|:---|:---|
|const bzd::StringView &|str||
------
### `constexpr void onError(const bzd::StringView & message) const`

#### Parameters
||||
|---:|:---|:---|
|const bzd::StringView &|message||
