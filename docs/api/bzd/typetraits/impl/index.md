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
|[`Decay`](decay/index.md)||
|[`EnableIf`](enableif/index.md)||
|[`EnableIf< true, T >`](enableif_true_t_/index.md)||
|[`IntegralConstant`](integralconstant/index.md)||
|[`IsArithmetic`](isarithmetic/index.md)||
|[`IsArray`](isarray/index.md)||
|[`IsArray< T[]>`](isarray_t_/index.md)||
|[`IsArray< T[N]>`](isarray_t_n_/index.md)||
|[`IsConst`](isconst/index.md)||
|[`IsConst< const T >`](isconst_const_t_/index.md)||
|[`IsConstructible`](isconstructible/index.md)||
|[`IsConstructible< VoidType< decltype(T(declval< Args >()...))>, T, Args... >`](isconstructible_voidtype_decltype_t_declval_args_t_args_/index.md)||
|[`IsDestructible`](isdestructible/index.md)||
|[`IsDestructible< T & >`](isdestructible_t_/index.md)||
|[`IsDestructible< T && >`](isdestructible_t_/index.md)||
|[`IsDestructible< T[]>`](isdestructible_t_/index.md)||
|[`IsDestructible< T[N]>`](isdestructible_t_n_/index.md)||
|[`IsDestructible< void >`](isdestructible_void_/index.md)||
|[`IsDestructible< void const >`](isdestructible_void_const_/index.md)||
|[`IsDestructible< void const volatile >`](isdestructible_void_const_volatile_/index.md)||
|[`IsDestructible< void volatile >`](isdestructible_void_volatile_/index.md)||
|[`IsDestructibleHelper`](isdestructiblehelper/index.md)||
|[`IsFloatingPoint`](isfloatingpoint/index.md)||
|[`IsFunction`](isfunction/index.md)||
|[`IsFunction< Ret(Args...) && >`](isfunction_ret_args_/index.md)||
|[`IsFunction< Ret(Args...) const & >`](isfunction_ret_args_const_/index.md)||
|[`IsFunction< Ret(Args...) const && >`](isfunction_ret_args_const_/index.md)||
|[`IsFunction< Ret(Args...) const >`](isfunction_ret_args_const_/index.md)||
|[`IsFunction< Ret(Args...) const volatile & >`](isfunction_ret_args_const_volatile_/index.md)||
|[`IsFunction< Ret(Args...) const volatile && >`](isfunction_ret_args_const_volatile_/index.md)||
|[`IsFunction< Ret(Args...) const volatile >`](isfunction_ret_args_const_volatile_/index.md)||
|[`IsFunction< Ret(Args...) volatile & >`](isfunction_ret_args_volatile_/index.md)||
|[`IsFunction< Ret(Args...) volatile && >`](isfunction_ret_args_volatile_/index.md)||
|[`IsFunction< Ret(Args...) volatile >`](isfunction_ret_args_volatile_/index.md)||
|[`IsFunction< Ret(Args...)& >`](isfunction_ret_args_/index.md)||
|[`IsFunction< Ret(Args...)>`](isfunction_ret_args_/index.md)||
|[`IsIntegral`](isintegral/index.md)||
|[`IsLValueReference`](islvaluereference/index.md)||
|[`IsLValueReference< T & >`](islvaluereference_t_/index.md)||
|[`IsReference`](isreference/index.md)||
|[`IsRValueReference`](isrvaluereference/index.md)||
|[`IsRValueReference< T && >`](isrvaluereference_t_/index.md)||
|[`IsSame`](issame/index.md)||
|[`IsSame< T, T >`](issame_t_t_/index.md)||
|[`IsVoid`](isvoid/index.md)||
|[`IsVoid< const void >`](isvoid_const_void_/index.md)||
|[`IsVoid< const volatile void >`](isvoid_const_volatile_void_/index.md)||
|[`IsVoid< void >`](isvoid_void_/index.md)||
|[`IsVoid< volatile void >`](isvoid_volatile_void_/index.md)||
|[`IsVolatile`](isvolatile/index.md)||
|[`IsVolatile< volatile T >`](isvolatile_volatile_t_/index.md)||
|[`RemoveConst`](removeconst/index.md)||
|[`RemoveConst< const T >`](removeconst_const_t_/index.md)||
|[`RemoveExtent`](removeextent/index.md)||
|[`RemoveExtent< T[]>`](removeextent_t_/index.md)||
|[`RemoveExtent< T[N]>`](removeextent_t_n_/index.md)||
|[`RemoveReference`](removereference/index.md)||
|[`RemoveReference< T & >`](removereference_t_/index.md)||
|[`RemoveReference< T && >`](removereference_t_/index.md)||
|[`RemoveVolatile`](removevolatile/index.md)||
|[`RemoveVolatile< volatile T >`](removevolatile_volatile_t_/index.md)||
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
### `template<class T> struct Decay`

#### Template
||||
|---:|:---|:---|
|class T|None||
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
### `template<class T, T v> struct IntegralConstant`

#### Template
||||
|---:|:---|:---|
|class|T||
|T|v||
------
### `template<class T> struct IsArithmetic`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct IsArray`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct IsArray< T[]>`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T, unsigned long int N> struct IsArray< T[N]>`

#### Template
||||
|---:|:---|:---|
|class T|None||
|unsigned long int|N||
------
### `template<class T> struct IsConst`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct IsConst< const T >`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class, class T, class... Args> struct IsConstructible`

#### Template
||||
|---:|:---|:---|
|class|None||
|class|T||
|class...|Args||
------
### `template<class T, class... Args> struct IsConstructible< VoidType< decltype(T(declval< Args >()...))>, T, Args... >`

#### Template
||||
|---:|:---|:---|
|class T|None||
|class...|Args||
------
### `template<class T> struct IsDestructible`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct IsDestructible< T & >`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct IsDestructible< T && >`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct IsDestructible< T[]>`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T, unsigned long int N> struct IsDestructible< T[N]>`

#### Template
||||
|---:|:---|:---|
|class T|None||
|unsigned long int|N||
------
### `struct IsDestructible< void >`

------
### `struct IsDestructible< void const >`

------
### `struct IsDestructible< void const volatile >`

------
### `struct IsDestructible< void volatile >`

------
### `struct IsDestructibleHelper`

------
### `template<class T> struct IsFloatingPoint`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class> struct IsFunction`

#### Template
||||
|---:|:---|:---|
|class|None||
------
### `template<class Ret, class... Args> struct IsFunction< Ret(Args...) && >`

#### Template
||||
|---:|:---|:---|
|class Ret|None||
|class...|Args||
------
### `template<class Ret, class... Args> struct IsFunction< Ret(Args...) const & >`

#### Template
||||
|---:|:---|:---|
|class Ret|None||
|class...|Args||
------
### `template<class Ret, class... Args> struct IsFunction< Ret(Args...) const && >`

#### Template
||||
|---:|:---|:---|
|class Ret|None||
|class...|Args||
------
### `template<class Ret, class... Args> struct IsFunction< Ret(Args...) const >`

#### Template
||||
|---:|:---|:---|
|class Ret|None||
|class...|Args||
------
### `template<class Ret, class... Args> struct IsFunction< Ret(Args...) const volatile & >`

#### Template
||||
|---:|:---|:---|
|class Ret|None||
|class...|Args||
------
### `template<class Ret, class... Args> struct IsFunction< Ret(Args...) const volatile && >`

#### Template
||||
|---:|:---|:---|
|class Ret|None||
|class...|Args||
------
### `template<class Ret, class... Args> struct IsFunction< Ret(Args...) const volatile >`

#### Template
||||
|---:|:---|:---|
|class Ret|None||
|class...|Args||
------
### `template<class Ret, class... Args> struct IsFunction< Ret(Args...) volatile & >`

#### Template
||||
|---:|:---|:---|
|class Ret|None||
|class...|Args||
------
### `template<class Ret, class... Args> struct IsFunction< Ret(Args...) volatile && >`

#### Template
||||
|---:|:---|:---|
|class Ret|None||
|class...|Args||
------
### `template<class Ret, class... Args> struct IsFunction< Ret(Args...) volatile >`

#### Template
||||
|---:|:---|:---|
|class Ret|None||
|class...|Args||
------
### `template<class Ret, class... Args> struct IsFunction< Ret(Args...)& >`

#### Template
||||
|---:|:---|:---|
|class Ret|None||
|class...|Args||
------
### `template<class Ret, class... Args> struct IsFunction< Ret(Args...)>`

#### Template
||||
|---:|:---|:---|
|class Ret|None||
|class...|Args||
------
### `template<class T> struct IsIntegral`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct IsLValueReference`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct IsLValueReference< T & >`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct IsReference`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct IsRValueReference`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct IsRValueReference< T && >`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T, class U> struct IsSame`

#### Template
||||
|---:|:---|:---|
|class T|None||
|class U|None||
------
### `template<class T> struct IsSame< T, T >`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct IsVoid`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `struct IsVoid< const void >`

------
### `struct IsVoid< const volatile void >`

------
### `struct IsVoid< void >`

------
### `struct IsVoid< volatile void >`

------
### `template<class T> struct IsVolatile`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct IsVolatile< volatile T >`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct RemoveConst`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct RemoveConst< const T >`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct RemoveExtent`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct RemoveExtent< T[]>`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T, unsigned long int N> struct RemoveExtent< T[N]>`

#### Template
||||
|---:|:---|:---|
|class T|None||
|unsigned long int|N||
------
### `template<class T> struct RemoveReference`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct RemoveReference< T & >`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct RemoveReference< T && >`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct RemoveVolatile`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct RemoveVolatile< volatile T >`

#### Template
||||
|---:|:---|:---|
|class T|None||
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
