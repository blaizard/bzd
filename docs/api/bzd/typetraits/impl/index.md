# [`bzd`](../../../index.md)::[`typeTraits`](../../index.md)::[`impl`](../index.md)

## ` impl`


|Struct||
|:---|:---|
|[`addRValueReference`](addrvaluereference/index.md)||
|[`addRValueReferenceHelper`](addrvaluereferencehelper/index.md)||
|[`addRValueReferenceHelper< T, true >`](addrvaluereferencehelper_t_true_/index.md)||
|[`isConstructible`](isconstructible/index.md)||
|[`isConstructible< voidType< decltype(T(declval< Args >()...))>, T, Args... >`](isconstructible_voidtype_decltype_t_declval_args_t_args_/index.md)||
|[`isDestructible`](isdestructible/index.md)||
|[`TypeIdentity`](typeidentity/index.md)||

|Function||
|:---|:---|
|[`tryAddPointer(int)`](./index.md)||
|[`tryAddPointer(...)`](./index.md)||
------
### `template<typename T> struct addRValueReference`

#### Template
||||
|---:|:---|:---|
|typename T|None||
------
### `template<typename T, bool b> struct addRValueReferenceHelper`

#### Template
||||
|---:|:---|:---|
|typename T|None||
|bool|b||
------
### `template<typename T> struct addRValueReferenceHelper< T, true >`

#### Template
||||
|---:|:---|:---|
|typename T|None||
------
### `template<class, class T, class... Args> struct isConstructible`

#### Template
||||
|---:|:---|:---|
|class|None||
|class|T||
|class...|Args||
------
### `template<class T, class... Args> struct isConstructible< voidType< decltype(T(declval< Args >()...))>, T, Args... >`

#### Template
||||
|---:|:---|:---|
|class T|None||
|class...|Args||
------
### `struct isDestructible`

------
### `template<class T> struct TypeIdentity`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> auto tryAddPointer(int)`

#### Template
||||
|---:|:---|:---|
|class T|None||
#### Parameters
||||
|---:|:---|:---|
|int|None||
------
### `template<class T> auto tryAddPointer(...)`

#### Template
||||
|---:|:---|:---|
|class T|None||
#### Parameters
||||
|---:|:---|:---|
|...|None||
