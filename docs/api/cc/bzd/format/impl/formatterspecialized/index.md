# [`bzd`](../../../../index.md)::[`format`](../../../index.md)::[`impl`](../../index.md)::[`FormatterSpecialized`](../index.md)

## `template<class T> class FormatterSpecialized`

#### Template
||||
|---:|:---|:---|
|class T|None||

|Function||
|:---|:---|
|[`FormatterSpecialized(const T & v)`](./index.md)||
|[`print(bzd::OChannel & os, const Metadata & metadata) const`](./index.md)||
|[`print(bzd::OChannel & os, const Metadata & metadata) const`](./index.md)||
|[`printToString(bzd::OChannel & os, const Metadata &) const`](./index.md)||
|[`printToString(bzd::OChannel & os, const Metadata & metadata) const`](./index.md)||
------
### `explicit constexpr FormatterSpecialized(const T & v)`

#### Parameters
||||
|---:|:---|:---|
|const T &|v||
------
### `virtual void print(bzd::OChannel & os, const Metadata & metadata) const`

#### Parameters
||||
|---:|:---|:---|
|bzd::OChannel &|os||
|const Metadata &|metadata||
------
### `virtual void print(bzd::OChannel & os, const Metadata & metadata) const`
*From bzd::format::impl::Formatter*


#### Parameters
||||
|---:|:---|:---|
|bzd::OChannel &|os||
|const Metadata &|metadata||
------
### `template<class U, bzd::typeTraits::EnableIf<!HasFormatterWithMetadata< U >::value, void > *> void printToString(bzd::OChannel & os, const Metadata &) const`

#### Template
||||
|---:|:---|:---|
|class U|None||
|bzd::typeTraits::EnableIf<!HasFormatterWithMetadata< U >::value, void > *|None||
#### Parameters
||||
|---:|:---|:---|
|bzd::OChannel &|os||
|const Metadata &|None||
------
### `template<class U, bzd::typeTraits::EnableIf< HasFormatterWithMetadata< U >::value, void > *> void printToString(bzd::OChannel & os, const Metadata & metadata) const`

#### Template
||||
|---:|:---|:---|
|class U|None||
|bzd::typeTraits::EnableIf< HasFormatterWithMetadata< U >::value, void > *|None||
#### Parameters
||||
|---:|:---|:---|
|bzd::OChannel &|os||
|const Metadata &|metadata||
