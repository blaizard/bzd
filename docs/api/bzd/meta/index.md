# [`bzd`](../../index.md)::[`meta`](../index.md)

## ` meta`


|Namespace||
|:---|:---|
|[`impl`](impl/index.md)||

|Struct||
|:---|:---|
|[`Type`](type/index.md)||
|[`TypeList`](typelist/index.md)||

|Typedef||
|:---|:---|
|[`ChooseNth`](./index.md)|alias of [`bzd::meta::impl::ChooseNth`](impl/choosenth/index.md)|
|[`Contains`](./index.md)|alias of [`bzd::meta::impl::Contains`](impl/contains/index.md)|
|[`Find`](./index.md)|alias of [`bzd::meta::impl::Find`](impl/find/index.md)|
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
