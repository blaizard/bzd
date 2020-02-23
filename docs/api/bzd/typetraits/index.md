# [`bzd`](../../index.md)::[`typeTraits`](../index.md)

## ` typeTraits`


|Namespace||
|:---|:---|
|[`impl`](impl/index.md)||

|Struct||
|:---|:---|
|[`integralConstant`](integralconstant/index.md)||
|[`isArithmetic`](isarithmetic/index.md)||
|[`isArray`](isarray/index.md)||
|[`isArray< T[]>`](isarray_t_/index.md)||
|[`isArray< T[N]>`](isarray_t_n_/index.md)||
|[`isConst`](isconst/index.md)||
|[`isConst< const T >`](isconst_const_t_/index.md)||
|[`isDestructible`](isdestructible/index.md)||
|[`isDestructible< T & >`](isdestructible_t_/index.md)||
|[`isDestructible< T && >`](isdestructible_t_/index.md)||
|[`isDestructible< T[]>`](isdestructible_t_/index.md)||
|[`isDestructible< T[N]>`](isdestructible_t_n_/index.md)||
|[`isDestructible< void >`](isdestructible_void_/index.md)||
|[`isDestructible< void const >`](isdestructible_void_const_/index.md)||
|[`isDestructible< void const volatile >`](isdestructible_void_const_volatile_/index.md)||
|[`isDestructible< void volatile >`](isdestructible_void_volatile_/index.md)||
|[`isFloatingPoint`](isfloatingpoint/index.md)||
|[`isFunction`](isfunction/index.md)||
|[`isFunction< Ret(Args...) && >`](isfunction_ret_args_/index.md)||
|[`isFunction< Ret(Args...) const & >`](isfunction_ret_args_const_/index.md)||
|[`isFunction< Ret(Args...) const && >`](isfunction_ret_args_const_/index.md)||
|[`isFunction< Ret(Args...) const >`](isfunction_ret_args_const_/index.md)||
|[`isFunction< Ret(Args...) const volatile & >`](isfunction_ret_args_const_volatile_/index.md)||
|[`isFunction< Ret(Args...) const volatile && >`](isfunction_ret_args_const_volatile_/index.md)||
|[`isFunction< Ret(Args...) const volatile >`](isfunction_ret_args_const_volatile_/index.md)||
|[`isFunction< Ret(Args...) volatile & >`](isfunction_ret_args_volatile_/index.md)||
|[`isFunction< Ret(Args...) volatile && >`](isfunction_ret_args_volatile_/index.md)||
|[`isFunction< Ret(Args...) volatile >`](isfunction_ret_args_volatile_/index.md)||
|[`isFunction< Ret(Args...)& >`](isfunction_ret_args_/index.md)||
|[`isFunction< Ret(Args...)>`](isfunction_ret_args_/index.md)||
|[`isIntegral`](isintegral/index.md)||
|[`isLValueReference`](islvaluereference/index.md)||
|[`isLValueReference< T & >`](islvaluereference_t_/index.md)||
|[`isReference`](isreference/index.md)||
|[`isRValueReference`](isrvaluereference/index.md)||
|[`isRValueReference< T && >`](isrvaluereference_t_/index.md)||
|[`isSame`](issame/index.md)||
|[`isSame< T, T >`](issame_t_t_/index.md)||
|[`isVoid`](isvoid/index.md)||
|[`isVoid< const void >`](isvoid_const_void_/index.md)||
|[`isVoid< const volatile void >`](isvoid_const_volatile_void_/index.md)||
|[`isVoid< void >`](isvoid_void_/index.md)||
|[`isVoid< volatile void >`](isvoid_volatile_void_/index.md)||
|[`isVolatile`](isvolatile/index.md)||
|[`isVolatile< volatile T >`](isvolatile_volatile_t_/index.md)||
|[`removeConst`](removeconst/index.md)||
|[`removeConst< const T >`](removeconst_const_t_/index.md)||
|[`removeCV`](removecv/index.md)||
|[`removeExtent`](removeextent/index.md)||
|[`removeExtent< T[]>`](removeextent_t_/index.md)||
|[`removeExtent< T[N]>`](removeextent_t_n_/index.md)||
|[`removeReference`](removereference/index.md)||
|[`removeReference< T & >`](removereference_t_/index.md)||
|[`removeReference< T && >`](removereference_t_/index.md)||
|[`removeVolatile`](removevolatile/index.md)||
|[`removeVolatile< volatile T >`](removevolatile_volatile_t_/index.md)||

|Function||
|:---|:---|
|[`declval()`](./index.md)||

|Typedef||
|:---|:---|
|[`AddConst`](./index.md)|alias of [`bzd::typeTraits::impl::AddConst`](impl/addconst/index.md)|
|[`AddCV`](./index.md)|alias of [`bzd::typeTraits::impl::AddCV`](impl/addcv/index.md)|
|[`AddLValueReference`](./index.md)|alias of [`bzd::typeTraits::impl::AddLValueReference`](impl/addlvaluereference/index.md)|
|[`AddPointer`](./index.md)|alias of [`bzd::typeTraits::impl::AddPointer`](impl/addpointer/index.md)|
|[`AddReference`](./index.md)|alias of [`bzd::typeTraits::impl::AddReference`](impl/addreference/index.md)|
|[`AddRValueReference`](./index.md)|alias of [`bzd::typeTraits::impl::AddRValueReference`](impl/addrvaluereference/index.md)|
|[`AddVolatile`](./index.md)|alias of [`bzd::typeTraits::impl::AddVolatile`](impl/addvolatile/index.md)|
|[`Conditional`](./index.md)|alias of [`bzd::typeTraits::impl::Conditional`](impl/conditional/index.md)|
|[`EnableIf`](./index.md)|alias of [`bzd::typeTraits::impl::EnableIf`](impl/enableif/index.md)|
|[`falseType`](./index.md)|alias of [`bzd::typeTraits::integralConstant`](integralconstant/index.md)|
|[`isConstructible`](./index.md)|alias of [`bzd::typeTraits::impl::isConstructible`](impl/isconstructible/index.md)|
|[`trueType`](./index.md)|alias of [`bzd::typeTraits::integralConstant`](integralconstant/index.md)|
|[`voidType`](./index.md)||
------
### `template<class T, T v> struct integralConstant`

#### Template
||||
|---:|:---|:---|
|class|T||
|T|v||
------
### `template<class T> struct isArithmetic`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct isArray`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct isArray< T[]>`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T, unsigned long int N> struct isArray< T[N]>`

#### Template
||||
|---:|:---|:---|
|class T|None||
|unsigned long int|N||
------
### `template<class T> struct isConst`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct isConst< const T >`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct isDestructible`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct isDestructible< T & >`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct isDestructible< T && >`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct isDestructible< T[]>`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T, unsigned long int N> struct isDestructible< T[N]>`

#### Template
||||
|---:|:---|:---|
|class T|None||
|unsigned long int|N||
------
### `struct isDestructible< void >`

------
### `struct isDestructible< void const >`

------
### `struct isDestructible< void const volatile >`

------
### `struct isDestructible< void volatile >`

------
### `template<class T> struct isFloatingPoint`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class> struct isFunction`

#### Template
||||
|---:|:---|:---|
|class|None||
------
### `template<class Ret, class... Args> struct isFunction< Ret(Args...) && >`

#### Template
||||
|---:|:---|:---|
|class Ret|None||
|class...|Args||
------
### `template<class Ret, class... Args> struct isFunction< Ret(Args...) const & >`

#### Template
||||
|---:|:---|:---|
|class Ret|None||
|class...|Args||
------
### `template<class Ret, class... Args> struct isFunction< Ret(Args...) const && >`

#### Template
||||
|---:|:---|:---|
|class Ret|None||
|class...|Args||
------
### `template<class Ret, class... Args> struct isFunction< Ret(Args...) const >`

#### Template
||||
|---:|:---|:---|
|class Ret|None||
|class...|Args||
------
### `template<class Ret, class... Args> struct isFunction< Ret(Args...) const volatile & >`

#### Template
||||
|---:|:---|:---|
|class Ret|None||
|class...|Args||
------
### `template<class Ret, class... Args> struct isFunction< Ret(Args...) const volatile && >`

#### Template
||||
|---:|:---|:---|
|class Ret|None||
|class...|Args||
------
### `template<class Ret, class... Args> struct isFunction< Ret(Args...) const volatile >`

#### Template
||||
|---:|:---|:---|
|class Ret|None||
|class...|Args||
------
### `template<class Ret, class... Args> struct isFunction< Ret(Args...) volatile & >`

#### Template
||||
|---:|:---|:---|
|class Ret|None||
|class...|Args||
------
### `template<class Ret, class... Args> struct isFunction< Ret(Args...) volatile && >`

#### Template
||||
|---:|:---|:---|
|class Ret|None||
|class...|Args||
------
### `template<class Ret, class... Args> struct isFunction< Ret(Args...) volatile >`

#### Template
||||
|---:|:---|:---|
|class Ret|None||
|class...|Args||
------
### `template<class Ret, class... Args> struct isFunction< Ret(Args...)& >`

#### Template
||||
|---:|:---|:---|
|class Ret|None||
|class...|Args||
------
### `template<class Ret, class... Args> struct isFunction< Ret(Args...)>`

#### Template
||||
|---:|:---|:---|
|class Ret|None||
|class...|Args||
------
### `template<class T> struct isIntegral`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct isLValueReference`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct isLValueReference< T & >`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct isReference`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct isRValueReference`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct isRValueReference< T && >`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T, class U> struct isSame`

#### Template
||||
|---:|:---|:---|
|class T|None||
|class U|None||
------
### `template<class T> struct isSame< T, T >`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct isVoid`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `struct isVoid< const void >`

------
### `struct isVoid< const volatile void >`

------
### `struct isVoid< void >`

------
### `struct isVoid< volatile void >`

------
### `template<class T> struct isVolatile`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct isVolatile< volatile T >`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct removeConst`

#### Template
||||
|---:|:---|:---|
|class|T||
------
### `template<class T> struct removeConst< const T >`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct removeCV`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct removeExtent`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct removeExtent< T[]>`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T, unsigned long int N> struct removeExtent< T[N]>`

#### Template
||||
|---:|:---|:---|
|class T|None||
|unsigned long int|N||
------
### `template<class T> struct removeReference`

#### Template
||||
|---:|:---|:---|
|class|T||
------
### `template<class T> struct removeReference< T & >`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct removeReference< T && >`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct removeVolatile`

#### Template
||||
|---:|:---|:---|
|class|T||
------
### `template<class T> struct removeVolatile< volatile T >`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> AddRValueReference< T > declval()`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> typedef AddConst`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> typedef AddCV`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> typedef AddLValueReference`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> typedef AddPointer`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> typedef AddReference`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> typedef AddRValueReference`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> typedef AddVolatile`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<bool Condition, class T, class F> typedef Conditional`

#### Template
||||
|---:|:---|:---|
|bool|Condition||
|class T|None||
|class F|None||
------
### `template<bool Condition, class T> typedef EnableIf`

#### Template
||||
|---:|:---|:---|
|bool|Condition||
|class T|None||
------
### `typedef falseType`

------
### `template<class T, class... Args> typedef isConstructible`

#### Template
||||
|---:|:---|:---|
|class T|None||
|class...|Args||
------
### `typedef trueType`

------
### `template<class...> typedef voidType`

#### Template
||||
|---:|:---|:---|
|class...|None||
