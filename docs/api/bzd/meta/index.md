# [`bzd`](../../index.md)::[`meta`](../index.md)

## ` meta`

### Namespace
||||
|---:|:---|:---|
||[`impl`](./impl/index.md)||
### Struct
||||
|---:|:---|:---|
|struct|[`TypeList`](./typelist/index.md)||
|struct|[`Type`](./type/index.md)||
### Typedef
||||
|---:|:---|:---|
|typedef|[`ChooseNth`](.)|alias of [`bzd::meta::impl::ChooseNth`](impl/choosenth/index.md)|
|typedef|[`Contains`](.)|alias of [`bzd::meta::impl::Contains`](impl/contains/index.md)|
|typedef|[`Find`](.)|alias of [`bzd::meta::impl::Find`](impl/find/index.md)|
|typedef|[`Union`](.)||
|typedef|[`UnionConstexpr`](.)||
------
### `template<class... Ts> struct TypeList`

#### Template
||||
|---:|:---|:---|
|class...|Ts||
------
### `template<class T> struct Type`

#### Template
||||
|---:|:---|:---|
|class|T||
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
