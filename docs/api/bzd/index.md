# [`bzd`](../index.md)

## ` bzd`


|Namespace||
|:---|:---|
|[`format`](format/index.md)||
|[`impl`](impl/index.md)||
|[`typeTraits`](typetraits/index.md)||
|[`meta`](meta/index.md)||
|[`iterator`](iterator/index.md)||
|[`interface`](interface/index.md)||
|[`log`](log/index.md)||
|[`assert`](assert/index.md)||

|Class||
|:---|:---|
|[`Task`](task/index.md)||
|[`Scheduler`](scheduler/index.md)||
|[`Variant`](variant/index.md)||
|[`Array`](array/index.md)|A container that encapsulates fixed size arrays.|
|[`BTree`](btree/index.md)||
|[`StringStream`](stringstream/index.md)||
|[`Tuple`](tuple/index.md)||
|[`Span`](span/index.md)|The class template span describes an object that can refer to a contiguous sequence of objects with the first element of the sequence at position zero.|
|[`SingletonThreadLocal`](singletonthreadlocal/index.md)||
|[`ReferenceWrapper`](referencewrapper/index.md)||
|[`String`](string/index.md)||
|[`Pool`](pool/index.md)||
|[`VariantConstexpr`](variantconstexpr/index.md)||
|[`Registry`](registry/index.md)||
|[`Singleton`](singleton/index.md)||
|[`Stack`](stack/index.md)||
|[`Queue`](queue/index.md)||
|[`Map`](map/index.md)||
|[`Registry< T, static_cast< SizeType >(-1)>`](registry_t_static_cast_sizetype_1_/index.md)||
|[`Vector`](vector/index.md)||

|Struct||
|:---|:---|
|[`resultOf< T(&)(Args &&...)>`](resultof_t_args_/index.md)||
|[`resultOf`](resultof/index.md)||
|[`decay`](decay/index.md)||
|[`alignedStorage`](alignedstorage/index.md)||

|Function||
|:---|:---|
|[`makeUnexpected(E && e)`](./index.md)||
|[`panic()`](./index.md)||
|[`getOut()`](./index.md)||
|[`yield()`](./index.md)||
|[`containerOf(M * ptr, const M T::* member)`](./index.md)||
|[`forward(typename typeTraits::removeReference< T >::type & t)`](./index.md)||
|[`forward(typename typeTraits::removeReference< T >::type && t)`](./index.md)||
|[`max(const T & a)`](./index.md)||
|[`max(const T & a, const T & b, const Ts &... n)`](./index.md)||
|[`memcpy(char * dest, const char * src, const SizeType size)`](./index.md)||
|[`min(const T & a)`](./index.md)||
|[`min(const T & a, const T & b, const Ts &... n)`](./index.md)||
|[`move(T && arg)`](./index.md)||
|[`offsetOf(const M T::* member)`](./index.md)||
|[`swap(T & t1, T & t2)`](./index.md)||

|Typedef||
|:---|:---|
|[`Expected`](./index.md)|This is the type used for returning and propagating errors.; alias of [`bzd::impl::Expected`](impl/expected/index.md)|
|[`OStream`](./index.md)|alias of [`bzd::impl::OStream`](impl/ostream/index.md)|
|[`IStream`](./index.md)|alias of [`bzd::impl::IStream`](impl/istream/index.md)|
|[`IOStream`](./index.md)|alias of [`bzd::impl::IOStream`](impl/iostream/index.md)|
|[`Optional`](./index.md)|Type managing an optional contained value, i.e. a value that may or may not be present.; alias of [`bzd::impl::Optional`](impl/optional/index.md)|
|[`StringView`](./index.md)|alias of [`bzd::impl::StringView`](impl/stringview/index.md)|
|[`PtrType`](./index.md)|Memory pointer type.|
|[`IntPtrType`](./index.md)|Integer type capable of holding a memory pointer.|
|[`FctPtrType`](./index.md)|Function pointer type.|
|[`SizeType`](./index.md)|Unsigned integer that can store the maximum size of a theoretically possible object of any type.|
|[`Int8Type`](./index.md)|Signed integer type with a fixed width of 8-bit.|
|[`Int16Type`](./index.md)|Signed integer type with a fixed width of 16-bit.|
|[`Int32Type`](./index.md)|Signed integer type with a fixed width of 32-bit.|
|[`Int64Type`](./index.md)|Signed integer type with a fixed width of 64-bit.|
|[`UInt8Type`](./index.md)|Unsigned integer type with a fixed width of 8-bit.|
|[`UInt16Type`](./index.md)|Unsigned integer type with a fixed width of 16-bit.|
|[`UInt32Type`](./index.md)|Unsigned integer type with a fixed width of 32-bit.|
|[`UInt64Type`](./index.md)|Unsigned integer type with a fixed width of 64-bit.|
------
### `template<class T, class... Args> class Task`

#### Template
||||
|---:|:---|:---|
|class T|None||
|class...|Args||
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
|class T|None||
|SizeType|N||
------
### `template<class K, class V, SizeType N, SizeType Order> class BTree`

#### Template
||||
|---:|:---|:---|
|class K|None||
|class V|None||
|SizeType|N||
|SizeType|Order||
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
### `template<class T> class Span`
The class template span describes an object that can refer to a contiguous sequence of objects with the first element of the sequence at position zero.
#### Template
||||
|---:|:---|:---|
|class|T|Element type, must be a complete type that is not an abstract class type. |
------
### `template<class T> class SingletonThreadLocal`

#### Template
||||
|---:|:---|:---|
|class|T||
------
### `template<class T> class ReferenceWrapper`

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
### `template<class T, SizeType N, class CapacityType> class Pool`
A Pool is a fixed memory buffer containing fixed size elements that can be taken or released.
#### Template
||||
|---:|:---|:---|
|class|T||
|SizeType|N||
|class|CapacityType||
------
### `template<class... Ts> class VariantConstexpr`

#### Template
||||
|---:|:---|:---|
|class...|Ts||
------
### `template<class T, SizeType Capacity> class Registry`

#### Template
||||
|---:|:---|:---|
|class T|None||
|SizeType|Capacity||
------
### `template<class T> class Singleton`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<const SizeType N> class Stack`

#### Template
||||
|---:|:---|:---|
|const SizeType|N||
------
### `template<class T, SizeType N, class CapacityType> class Queue`

#### Template
||||
|---:|:---|:---|
|class|T||
|SizeType|N||
|class|CapacityType||
------
### `template<class K, class V, SizeType N> class Map`

#### Template
||||
|---:|:---|:---|
|class|K||
|class|V||
|SizeType|N||
------
### `template<class T> class Registry< T, static_cast< SizeType >(-1)>`

#### Template
||||
|---:|:---|:---|
|class T|None||
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
### `void panic()`

------
### `bzd::OStream & getOut()`

------
### `void yield()`

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
### `template<class T> constexpr T max(const T & a)`

#### Template
||||
|---:|:---|:---|
|class T|None||
#### Parameters
||||
|---:|:---|:---|
|const T &|a||
------
### `template<class T, class... Ts> constexpr T max(const T & a, const T & b, const Ts &... n)`

#### Template
||||
|---:|:---|:---|
|class T|None||
|class...|Ts||
#### Parameters
||||
|---:|:---|:---|
|const T &|a||
|const T &|b||
|const Ts &...|n||
------
### `constexpr void memcpy(char * dest, const char * src, const SizeType size)`

#### Parameters
||||
|---:|:---|:---|
|char *|dest||
|const char *|src||
|const SizeType|size||
------
### `template<class T> constexpr T min(const T & a)`

#### Template
||||
|---:|:---|:---|
|class T|None||
#### Parameters
||||
|---:|:---|:---|
|const T &|a||
------
### `template<class T, class... Ts> constexpr T min(const T & a, const T & b, const Ts &... n)`

#### Template
||||
|---:|:---|:---|
|class T|None||
|class...|Ts||
#### Parameters
||||
|---:|:---|:---|
|const T &|a||
|const T &|b||
|const Ts &...|n||
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
### `template<class T> typedef Optional`
Type managing an optional contained value, i.e. a value that may or may not be present.
#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `typedef StringView`

------
### `typedef PtrType`
Memory pointer type.
------
### `typedef IntPtrType`
Integer type capable of holding a memory pointer.
------
### `typedef FctPtrType`
Function pointer type.
------
### `typedef SizeType`
Unsigned integer that can store the maximum size of a theoretically possible object of any type.
------
### `typedef Int8Type`
Signed integer type with a fixed width of 8-bit.
------
### `typedef Int16Type`
Signed integer type with a fixed width of 16-bit.
------
### `typedef Int32Type`
Signed integer type with a fixed width of 32-bit.
------
### `typedef Int64Type`
Signed integer type with a fixed width of 64-bit.
------
### `typedef UInt8Type`
Unsigned integer type with a fixed width of 8-bit.
------
### `typedef UInt16Type`
Unsigned integer type with a fixed width of 16-bit.
------
### `typedef UInt32Type`
Unsigned integer type with a fixed width of 32-bit.
------
### `typedef UInt64Type`
Unsigned integer type with a fixed width of 64-bit.
