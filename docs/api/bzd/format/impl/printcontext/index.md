# [`bzd`](../../../../index.md)::[`format`](../../../index.md)::[`impl`](../../index.md)::[`PrintContext`](../index.md)

## `class PrintContext`


|Function||
|:---|:---|
|[`PrintContext(bzd::OStream & stream, const bzd::interface::Vector< bzd::format::impl::Arg > & args)`](./index.md)||
|[`addSubstring(const bzd::StringView & str)`](./index.md)||
|[`addMetadata(const Metadata & metadata)`](./index.md)||
|[`onError(const bzd::StringView & message) const`](./index.md)||
------
### `constexpr PrintContext(bzd::OStream & stream, const bzd::interface::Vector< bzd::format::impl::Arg > & args)`

#### Parameters
||||
|---:|:---|:---|
|bzd::OStream &|stream||
|const bzd::interface::Vector< bzd::format::impl::Arg > &|args||
------
### `void addSubstring(const bzd::StringView & str)`

#### Parameters
||||
|---:|:---|:---|
|const bzd::StringView &|str||
------
### `void addMetadata(const Metadata & metadata)`

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
