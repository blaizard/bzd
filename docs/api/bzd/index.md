# [`bzd`](../index.md)

## ` bzd`

### Namespace
||||
|---:|:---|:---|
||[`format`](format/index.md)||
||[`impl`](impl/index.md)||
||[`typeTraits`](typetraits/index.md)||
||[`meta`](meta/index.md)||
||[`iterator`](iterator/index.md)||
||[`interface`](interface/index.md)||
||[`log`](log/index.md)||
||[`assert`](assert/index.md)||
### Class
||||
|---:|:---|:---|
|class|[`Task`](task/index.md)||
|class|[`SingletonThreadLocalImpl`](singletonthreadlocalimpl/index.md)||
|class|[`Scheduler`](scheduler/index.md)||
|class|[`Variant`](variant/index.md)||
|class|[`Array`](array/index.md)|A container that encapsulates fixed size arrays.|
|class|[`StringStream`](stringstream/index.md)||
|class|[`Tuple`](tuple/index.md)||
|class|[`Pool`](pool/index.md)||
|class|[`Span`](span/index.md)|The class template span describes an object that can refer to a contiguous sequence of objects with the first element of the sequence at position zero.|
|class|[`SingletonImpl`](singletonimpl/index.md)||
|class|[`String`](string/index.md)||
|class|[`VariantConstexpr`](variantconstexpr/index.md)||
|class|[`Stack`](stack/index.md)||
|class|[`Vector`](vector/index.md)||
### Struct
||||
|---:|:---|:---|
|struct|[`resultOf< T(&)(Args &&...)>`](resultof_t_args_/index.md)||
|struct|[`resultOf`](resultof/index.md)||
|struct|[`decay`](decay/index.md)||
|struct|[`alignedStorage`](alignedstorage/index.md)||
### Function
||||
|---:|:---|:---|
|constexpr impl::Unexpected< typename bzd::decay< E >::type >|[`makeUnexpected(E && e)`](./index.md)||
|void|[`yield()`](./index.md)||
|void|[`panic()`](./index.md)||
|bzd::OStream &|[`getOut()`](./index.md)||
|constexpr IntPtrType|[`offsetOf(const M T::* member)`](./index.md)||
|constexpr T *|[`containerOf(M * ptr, const M T::* member)`](./index.md)||
|constexpr void|[`memcpy(char * dest, const char * src, const SizeType size)`](./index.md)||
|constexpr T &&|[`forward(typename typeTraits::removeReference< T >::type & t)`](./index.md)||
|constexpr T &&|[`forward(typename typeTraits::removeReference< T >::type && t)`](./index.md)||
|bzd::typeTraits::removeReference< T >::type &&|[`move(T && arg)`](./index.md)||
|constexpr void|[`swap(T & t1, T & t2)`](./index.md)||
|constexpr T|[`min(const T & a, const T & b)`](./index.md)||
|constexpr T|[`max(const T & a, const T & b)`](./index.md)||
### Typedef
||||
|---:|:---|:---|
|typedef|[`Expected`](./index.md)|This is the type used for returning and propagating errors.; alias of [`bzd::impl::Expected`](impl/expected/index.md)|
|typedef|[`OStream`](./index.md)|alias of [`bzd::impl::OStream`](impl/ostream/index.md)|
|typedef|[`IStream`](./index.md)|alias of [`bzd::impl::IStream`](impl/istream/index.md)|
|typedef|[`IOStream`](./index.md)|alias of [`bzd::impl::IOStream`](impl/iostream/index.md)|
|typedef|[`StringView`](./index.md)|alias of [`bzd::impl::StringView`](impl/stringview/index.md)|
|typedef|[`PtrType`](./index.md)||
|typedef|[`PtrDiffType`](./index.md)||
|typedef|[`CtxPtrType`](./index.md)||
|typedef|[`IntPtrType`](./index.md)||
|typedef|[`FctPtrType`](./index.md)||
|typedef|[`ByteType`](./index.md)||
|typedef|[`SizeType`](./index.md)||
------
### `template<class T, class... Args> class Task`

#### Template
||||
|---:|:---|:---|
|class T|None||
|class...|Args||
------
### `template<class T> class SingletonThreadLocalImpl`

#### Template
||||
|---:|:---|:---|
|class|T||
------
### `class Scheduler`

------
### `template<class... Ts> class Variant`

#### Template
||||
|---:|:---|:---|
|class...|Ts||
------
### `template<class T, SizeType N> class Array`
A container that encapsulates fixed size arrays.

The container combines the performance and accessibility of a C-style array with the benefits of a standard container, such as knowing its own size, supporting assignment, random access iterators, etc. Unlike a C-style array, it doesn't decay to T* automatically.
#### Template
||||
|---:|:---|:---|
|class|T||
|SizeType|N||
------
### `template<SizeType N> class StringStream`

#### Template
||||
|---:|:---|:---|
|SizeType|N||
------
### `template<class... T> class Tuple`

#### Template
||||
|---:|:---|:---|
|class...|T||
------
### `template<class T, SizeType N> class Pool`
A Pool is a fixed memory buffer containing fixed size elements that can be taken or released.
#### Template
||||
|---:|:---|:---|
|class T|None||
|SizeType|N||
------
### `template<class T> class Span`
The class template span describes an object that can refer to a contiguous sequence of objects with the first element of the sequence at position zero.
#### Template
||||
|---:|:---|:---|
|class|T|Element type, must be a complete type that is not an abstract class type. |
------
### `template<class T> class SingletonImpl`
Singleton class pattern
#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<SizeType N> class String`

#### Template
||||
|---:|:---|:---|
|SizeType|N||
------
### `template<class... Ts> class VariantConstexpr`

#### Template
||||
|---:|:---|:---|
|class...|Ts||
------
### `template<const SizeType N> class Stack`

#### Template
||||
|---:|:---|:---|
|const SizeType|N||
------
### `template<class T, SizeType N> class Vector`

#### Template
||||
|---:|:---|:---|
|class|T||
|SizeType|N||
------
### `template<class T, class... Args> struct resultOf< T(&)(Args &&...)>`

#### Template
||||
|---:|:---|:---|
|class T|None||
|class...|Args||
------
### `template<class T> struct resultOf`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> struct decay`

#### Template
||||
|---:|:---|:---|
|class|T||
------
### `template<SizeType Len, SizeType Align> struct alignedStorage`

#### Template
||||
|---:|:---|:---|
|SizeType|Len||
|SizeType|Align||
------
### `template<class E> constexpr impl::Unexpected< typename bzd::decay< E >::type > makeUnexpected(E && e)`

#### Template
||||
|---:|:---|:---|
|class E|None||
#### Parameters
||||
|---:|:---|:---|
|E &&|e||
------
### `void yield()`

------
### `void panic()`

------
### `bzd::OStream & getOut()`

------
### `template<class T, class M> static constexpr IntPtrType offsetOf(const M T::* member)`

#### Template
||||
|---:|:---|:---|
|class T|None||
|class M|None||
#### Parameters
||||
|---:|:---|:---|
|const M T::*|member||
------
### `template<class T, class M> static constexpr T * containerOf(M * ptr, const M T::* member)`

#### Template
||||
|---:|:---|:---|
|class T|None||
|class M|None||
#### Parameters
||||
|---:|:---|:---|
|M *|ptr||
|const M T::*|member||
------
### `constexpr void memcpy(char * dest, const char * src, const SizeType size)`

#### Parameters
||||
|---:|:---|:---|
|char *|dest||
|const char *|src||
|const SizeType|size||
------
### `template<class T> constexpr T && forward(typename typeTraits::removeReference< T >::type & t)`

#### Template
||||
|---:|:---|:---|
|class T|None||
#### Parameters
||||
|---:|:---|:---|
|typename typeTraits::removeReference< T >::type &|t||
------
### `template<class T> constexpr T && forward(typename typeTraits::removeReference< T >::type && t)`

#### Template
||||
|---:|:---|:---|
|class T|None||
#### Parameters
||||
|---:|:---|:---|
|typename typeTraits::removeReference< T >::type &&|t||
------
### `template<class T> bzd::typeTraits::removeReference< T >::type && move(T && arg)`

#### Template
||||
|---:|:---|:---|
|class T|None||
#### Parameters
||||
|---:|:---|:---|
|T &&|arg||
------
### `template<class T> constexpr void swap(T & t1, T & t2)`

#### Template
||||
|---:|:---|:---|
|class T|None||
#### Parameters
||||
|---:|:---|:---|
|T &|t1||
|T &|t2||
------
### `template<class T> constexpr T min(const T & a, const T & b)`

#### Template
||||
|---:|:---|:---|
|class T|None||
#### Parameters
||||
|---:|:---|:---|
|const T &|a||
|const T &|b||
------
### `template<class T> constexpr T max(const T & a, const T & b)`

#### Template
||||
|---:|:---|:---|
|class T|None||
#### Parameters
||||
|---:|:---|:---|
|const T &|a||
|const T &|b||
------
### `template<class T, class E> typedef Expected`
This is the type used for returning and propagating errors.

It is a variants with 2 states, valid, representing success and containing a value, and error, representing error and containing an error value.
#### Template
||||
|---:|:---|:---|
|class T|None||
|class E|None||
------
### `typedef OStream`

------
### `typedef IStream`

------
### `typedef IOStream`

------
### `typedef StringView`

------
### `typedef PtrType`

------
### `typedef PtrDiffType`

------
### `typedef CtxPtrType`

------
### `typedef IntPtrType`

------
### `typedef FctPtrType`

------
### `typedef ByteType`

------
### `typedef SizeType`

