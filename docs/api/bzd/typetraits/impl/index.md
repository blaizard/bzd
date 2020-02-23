# [`bzd`](../../../index.md)::[`typeTraits`](../../index.md)::[`impl`](../index.md)

## ` impl`


|Struct||
|:---|:---|
|[`AddConst`](addconst/index.md)||
|[`AddCV`](addcv/index.md)||
|[`AddLValueReference`](addlvaluereference/index.md)||
|[`AddLValueReference< T && >`](addlvaluereference_t_/index.md)||
|[`AddPointer`](addpointer/index.md)||
|[`AddReference`](addreference/index.md)||
|[`AddReference< const void >`](addreference_const_void_/index.md)||
|[`AddReference< const volatile void >`](addreference_const_volatile_void_/index.md)||
|[`AddReference< T & >`](addreference_t_/index.md)||
|[`AddReference< T && >`](addreference_t_/index.md)||
|[`AddReference< void >`](addreference_void_/index.md)||
|[`AddReference< volatile void >`](addreference_volatile_void_/index.md)||
|[`AddRValueReference`](addrvaluereference/index.md)||
|[`AddRValueReferenceHelper`](addrvaluereferencehelper/index.md)||
|[`AddRValueReferenceHelper< T, true >`](addrvaluereferencehelper_t_true_/index.md)||
|[`AddVolatile`](addvolatile/index.md)||
|[`Conditional`](conditional/index.md)||
|[`Conditional< false, T, F >`](conditional_false_t_f_/index.md)||
|[`EnableIf`](enableif/index.md)||
|[`EnableIf< true, T >`](enableif_true_t_/index.md)||
|[`isConstructible`](isconstructible/index.md)||
|[`isConstructible< voidType< decltype(T(declval< Args >()...))>, T, Args... >`](isconstructible_voidtype_decltype_t_declval_args_t_args_/index.md)||
|[`isDestructible`](isdestructible/index.md)||
|[`TypeIdentity`](typeidentity/index.md)||

|Function||
|:---|:---|
|[`tryAddPointer(int)`](./index.md)||
|[`tryAddPointer(...)`](./index.md)||
------
### `template<class T> struct AddConst`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct AddCV`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct AddLValueReference`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct AddLValueReference< T && >`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct AddPointer`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct AddReference`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `struct AddReference< const void >`

------
### `struct AddReference< const volatile void >`

------
### `template<class T> struct AddReference< T & >`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct AddReference< T && >`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `struct AddReference< void >`

------
### `struct AddReference< volatile void >`

------
### `template<typename T> struct AddRValueReference`

#### Template
||||
|---:|:---|:---|
|typename T|None||
------
### `template<typename T, bool b> struct AddRValueReferenceHelper`

#### Template
||||
|---:|:---|:---|
|typename T|None||
|bool|b||
------
### `template<typename T> struct AddRValueReferenceHelper< T, true >`

#### Template
||||
|---:|:---|:---|
|typename T|None||
------
### `template<class T> struct AddVolatile`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<bool Condition, class T, class F> struct Conditional`

#### Template
||||
|---:|:---|:---|
|bool|Condition||
|class T|None||
|class F|None||
------
### `template<class T, class F> struct Conditional< false, T, F >`

#### Template
||||
|---:|:---|:---|
|class T|None||
|class F|None||
------
### `template<bool Condition, class T> struct EnableIf`

#### Template
||||
|---:|:---|:---|
|bool|Condition||
|class T|None||
------
### `template<class T> struct EnableIf< true, T >`

#### Template
||||
|---:|:---|:---|
|class T|None||
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
