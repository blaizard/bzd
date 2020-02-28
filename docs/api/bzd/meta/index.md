# [`bzd`](../../index.md)::[`meta`](../index.md)

## ` meta`


|Namespace||
|:---|:---|
|[`impl`](impl/index.md)||
|[`range`](range/index.md)||

|Struct||
|:---|:---|
|[`Type`](type/index.md)||
|[`TypeList`](typelist/index.md)||

|Typedef||
|:---|:---|
|[`ChooseNth`](./index.md)|alias of [`bzd::meta::impl::ChooseNth`](impl/choosenth/index.md)|
|[`Contains`](./index.md)|alias of [`bzd::meta::impl::Contains`](impl/contains/index.md)|
|[`Find`](./index.md)||
|[`FindConditional`](./index.md)|alias of [`bzd::meta::impl::FindConditional`](impl/findconditional/index.md)|
|[`Range`](./index.md)|alias of [`bzd::meta::impl::Range`](impl/range/index.md)|
|[`Union`](./index.md)||
|[`UnionConstexpr`](./index.md)||
------
### `template<class T> struct Type`

#### Template
||||
|---:|:---|:---|
|class|T||
------
### `template<class... Ts> struct TypeList`

#### Template
||||
|---:|:---|:---|
|class...|Ts||
------
### `template<SizeType N, class... Ts> typedef ChooseNth`

#### Template
||||
|---:|:---|:---|
|SizeType|N||
|class...|Ts||
------
### `template<class T, class... Ts> typedef Contains`

#### Template
||||
|---:|:---|:---|
|class T|None||
|class...|Ts||
------
### `template<class T, class... Ts> typedef Find`

#### Template
||||
|---:|:---|:---|
|class T|None||
|class...|Ts||
------
### `template<template< class, class > class Condition, class T, class... Ts> typedef FindConditional`

#### Template
||||
|---:|:---|:---|
|template< class, class > class|Condition||
|class T|None||
|class...|Ts||
------
### `template<SizeType Start, SizeType End> typedef Range`

#### Template
||||
|---:|:---|:---|
|SizeType|Start||
|SizeType|End||
------
### `template<class... Ts> typedef Union`

#### Template
||||
|---:|:---|:---|
|class...|Ts||
------
### `template<class... Ts> typedef UnionConstexpr`

#### Template
||||
|---:|:---|:---|
|class...|Ts||
