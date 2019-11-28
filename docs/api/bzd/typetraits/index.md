# [`bzd`](../../index.md)::[`typeTraits`](../index.md)

## ` typeTraits`


|Namespace||
|:---|:---|
|[`impl`](impl/index.md)||

|Struct||
|:---|:---|
|[`addReference< T & >`](addreference_t_/index.md)||
|[`addReference< void >`](addreference_void_/index.md)||
|[`isArithmetic`](isarithmetic/index.md)||
|[`isIntegral`](isintegral/index.md)||
|[`isDestructible< T[N]>`](isdestructible_t_n_/index.md)||
|[`isDestructible< void const >`](isdestructible_void_const_/index.md)||
|[`isArray< T[N]>`](isarray_t_n_/index.md)||
|[`isConst`](isconst/index.md)||
|[`isDestructible< T & >`](isdestructible_t_/index.md)||
|[`isFunction< Ret(Args...) const & >`](isfunction_ret_args_const_/index.md)||
|[`removeExtent< T[]>`](removeextent_t_/index.md)||
|[`isFunction< Ret(Args...) volatile & >`](isfunction_ret_args_volatile_/index.md)||
|[`isFunction< Ret(Args...) volatile >`](isfunction_ret_args_volatile_/index.md)||
|[`isReference`](isreference/index.md)||
|[`isDestructible< void const volatile >`](isdestructible_void_const_volatile_/index.md)||
|[`isSame`](issame/index.md)||
|[`removeConst`](removeconst/index.md)||
|[`addLValueReference`](addlvaluereference/index.md)||
|[`isFunction< Ret(Args...)>`](isfunction_ret_args_/index.md)||
|[`addConst`](addconst/index.md)||
|[`addReference`](addreference/index.md)||
|[`removeVolatile`](removevolatile/index.md)||
|[`isDestructible`](isdestructible/index.md)||
|[`isFunction< Ret(Args...) && >`](isfunction_ret_args_/index.md)||
|[`removeExtent< T[N]>`](removeextent_t_n_/index.md)||
|[`isLValueReference< T & >`](islvaluereference_t_/index.md)||
|[`isConst< const T >`](isconst_const_t_/index.md)||
|[`enableIf< true, T >`](enableif_true_t_/index.md)||
|[`isFunction< Ret(Args...) const volatile >`](isfunction_ret_args_const_volatile_/index.md)||
|[`isRValueReference`](isrvaluereference/index.md)||
|[`addVolatile`](addvolatile/index.md)||
|[`isVoid< const volatile void >`](isvoid_const_volatile_void_/index.md)||
|[`conditional`](conditional/index.md)||
|[`isLValueReference`](islvaluereference/index.md)||
|[`removeReference< T & >`](removereference_t_/index.md)||
|[`removeConst< const T >`](removeconst_const_t_/index.md)||
|[`removeExtent`](removeextent/index.md)||
|[`isVoid`](isvoid/index.md)||
|[`isDestructible< void volatile >`](isdestructible_void_volatile_/index.md)||
|[`isFunction< Ret(Args...) const >`](isfunction_ret_args_const_/index.md)||
|[`removeReference< T && >`](removereference_t_/index.md)||
|[`isDestructible< void >`](isdestructible_void_/index.md)||
|[`isArray`](isarray/index.md)||
|[`addReference< T && >`](addreference_t_/index.md)||
|[`enableIf`](enableif/index.md)||
|[`isFunction< Ret(Args...) volatile && >`](isfunction_ret_args_volatile_/index.md)||
|[`isFunction< Ret(Args...) const && >`](isfunction_ret_args_const_/index.md)||
|[`isDestructible< T && >`](isdestructible_t_/index.md)||
|[`isArray< T[]>`](isarray_t_/index.md)||
|[`addReference< const volatile void >`](addreference_const_volatile_void_/index.md)||
|[`addReference< volatile void >`](addreference_volatile_void_/index.md)||
|[`isVoid< void >`](isvoid_void_/index.md)||
|[`isDestructible< T[]>`](isdestructible_t_/index.md)||
|[`isVolatile`](isvolatile/index.md)||
|[`addPointer`](addpointer/index.md)||
|[`isFloatingPoint`](isfloatingpoint/index.md)||
|[`conditional< false, T, F >`](conditional_false_t_f_/index.md)||
|[`removeCV`](removecv/index.md)||
|[`isFunction< Ret(Args...) & >`](isfunction_ret_args_/index.md)||
|[`isVoid< const void >`](isvoid_const_void_/index.md)||
|[`isVoid< volatile void >`](isvoid_volatile_void_/index.md)||
|[`isFunction< Ret(Args...) const volatile & >`](isfunction_ret_args_const_volatile_/index.md)||
|[`isRValueReference< T && >`](isrvaluereference_t_/index.md)||
|[`isSame< T, T >`](issame_t_t_/index.md)||
|[`integralConstant`](integralconstant/index.md)||
|[`removeReference`](removereference/index.md)||
|[`isFunction`](isfunction/index.md)||
|[`isFunction< Ret(Args...) const volatile && >`](isfunction_ret_args_const_volatile_/index.md)||
|[`isVolatile< volatile T >`](isvolatile_volatile_t_/index.md)||
|[`addReference< const void >`](addreference_const_void_/index.md)||
|[`addLValueReference< T && >`](addlvaluereference_t_/index.md)||
|[`addRValueReference`](addrvaluereference/index.md)||
|[`removeVolatile< volatile T >`](removevolatile_volatile_t_/index.md)||
|[`addCV`](addcv/index.md)||

|Function||
|:---|:---|
|[`declval()`](./index.md)||

|Typedef||
|:---|:---|
|[`falseType`](./index.md)|alias of [`bzd::typeTraits::integralConstant`](integralconstant/index.md)|
|[`isConstructible`](./index.md)|alias of [`bzd::typeTraits::impl::isConstructible`](impl/isconstructible/index.md)|
|[`trueType`](./index.md)|alias of [`bzd::typeTraits::integralConstant`](integralconstant/index.md)|
|[`voidType`](./index.md)||
------
### `template<class T> struct addReference< T & >`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `struct addReference< void >`

------
### `template<class T> struct isArithmetic`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct isIntegral`

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
### `struct isDestructible< void const >`

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
### `template<class T> struct isDestructible< T & >`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class Ret, class... Args> struct isFunction< Ret(Args...) const & >`

#### Template
||||
|---:|:---|:---|
|class Ret|None||
|class...|Args||
------
### `template<class T> struct removeExtent< T[]>`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class Ret, class... Args> struct isFunction< Ret(Args...) volatile & >`

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
### `template<class T> struct isReference`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `struct isDestructible< void const volatile >`

------
### `template<class T, class U> struct isSame`

#### Template
||||
|---:|:---|:---|
|class T|None||
|class U|None||
------
### `template<class T> struct removeConst`

#### Template
||||
|---:|:---|:---|
|class|T||
------
### `template<class T> struct addLValueReference`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class Ret, class... Args> struct isFunction< Ret(Args...)>`

#### Template
||||
|---:|:---|:---|
|class Ret|None||
|class...|Args||
------
### `template<class T> struct addConst`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct addReference`

#### Template
||||
|---:|:---|:---|
|class|T||
------
### `template<class T> struct removeVolatile`

#### Template
||||
|---:|:---|:---|
|class|T||
------
### `template<class T> struct isDestructible`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class Ret, class... Args> struct isFunction< Ret(Args...) && >`

#### Template
||||
|---:|:---|:---|
|class Ret|None||
|class...|Args||
------
### `template<class T, unsigned long int N> struct removeExtent< T[N]>`

#### Template
||||
|---:|:---|:---|
|class T|None||
|unsigned long int|N||
------
### `template<class T> struct isLValueReference< T & >`

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
### `template<class T> struct enableIf< true, T >`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class Ret, class... Args> struct isFunction< Ret(Args...) const volatile >`

#### Template
||||
|---:|:---|:---|
|class Ret|None||
|class...|Args||
------
### `template<class T> struct isRValueReference`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct addVolatile`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `struct isVoid< const volatile void >`

------
### `template<bool B, class T, class F> struct conditional`

#### Template
||||
|---:|:---|:---|
|bool|B||
|class|T||
|class|F||
------
### `template<class T> struct isLValueReference`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct removeReference< T & >`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct removeConst< const T >`

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
### `template<class T> struct isVoid`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `struct isDestructible< void volatile >`

------
### `template<class Ret, class... Args> struct isFunction< Ret(Args...) const >`

#### Template
||||
|---:|:---|:---|
|class Ret|None||
|class...|Args||
------
### `template<class T> struct removeReference< T && >`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `struct isDestructible< void >`

------
### `template<class T> struct isArray`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct addReference< T && >`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<bool B, class T> struct enableIf`

#### Template
||||
|---:|:---|:---|
|bool|B||
|class T|None||
------
### `template<class Ret, class... Args> struct isFunction< Ret(Args...) volatile && >`

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
### `template<class T> struct isDestructible< T && >`

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
### `struct addReference< const volatile void >`

------
### `struct addReference< volatile void >`

------
### `struct isVoid< void >`

------
### `template<class T> struct isDestructible< T[]>`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct isVolatile`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct addPointer`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct isFloatingPoint`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T, class F> struct conditional< false, T, F >`

#### Template
||||
|---:|:---|:---|
|class T|None||
|class F|None||
------
### `template<class T> struct removeCV`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class Ret, class... Args> struct isFunction< Ret(Args...) & >`

#### Template
||||
|---:|:---|:---|
|class Ret|None||
|class...|Args||
------
### `struct isVoid< const void >`

------
### `struct isVoid< volatile void >`

------
### `template<class Ret, class... Args> struct isFunction< Ret(Args...) const volatile & >`

#### Template
||||
|---:|:---|:---|
|class Ret|None||
|class...|Args||
------
### `template<class T> struct isRValueReference< T && >`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct isSame< T, T >`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T, T v> struct integralConstant`

#### Template
||||
|---:|:---|:---|
|class|T||
|T|v||
------
### `template<class T> struct removeReference`

#### Template
||||
|---:|:---|:---|
|class|T||
------
### `template<class> struct isFunction`

#### Template
||||
|---:|:---|:---|
|class|None||
------
### `template<class Ret, class... Args> struct isFunction< Ret(Args...) const volatile && >`

#### Template
||||
|---:|:---|:---|
|class Ret|None||
|class...|Args||
------
### `template<class T> struct isVolatile< volatile T >`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `struct addReference< const void >`

------
### `template<class T> struct addLValueReference< T && >`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct addRValueReference`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct removeVolatile< volatile T >`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct addCV`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> addRValueReference< T >::type declval()`

#### Template
||||
|---:|:---|:---|
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
