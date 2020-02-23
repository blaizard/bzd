# [`bzd`](../index.md)

## ` bzd`


|Namespace||
|:---|:---|
|[`assert`](assert/index.md)||
|[`format`](format/index.md)||
|[`impl`](impl/index.md)||
|[`interface`](interface/index.md)||
|[`iterator`](iterator/index.md)||
|[`log`](log/index.md)||
|[`meta`](meta/index.md)||
|[`typeTraits`](typetraits/index.md)||

|Class||
|:---|:---|
|[`Array`](array/index.md)|A container that encapsulates fixed size arrays.|
|[`BTree`](btree/index.md)||
|[`Log`](log/index.md)||
|[`Map`](map/index.md)||
|[`Pool`](pool/index.md)||
|[`Queue`](queue/index.md)||
|[`ReferenceWrapper`](referencewrapper/index.md)||
|[`Registry`](registry/index.md)|Fixed-size registry object.|
|[`Scheduler`](scheduler/index.md)||
|[`Signal`](signal/index.md)||
|[`Singleton`](singleton/index.md)||
|[`SingletonThreadLocal`](singletonthreadlocal/index.md)||
|[`Span`](span/index.md)|The class template span describes an object that can refer to a contiguous sequence of objects with the first element of the sequence at position zero.|
|[`Stack`](stack/index.md)||
|[`String`](string/index.md)||
|[`StringStream`](stringstream/index.md)||
|[`Task`](task/index.md)||
|[`Tuple`](tuple/index.md)||
|[`Variant`](variant/index.md)||
|[`VariantConstexpr`](variantconstexpr/index.md)||
|[`Vector`](vector/index.md)||

|Struct||
|:---|:---|
|[`resultOf`](resultof/index.md)||
|[`resultOf< T(&)(Args &&...)>`](resultof_t_args_/index.md)||

|Function||
|:---|:---|
|[`containerOf(M * ptr, const M T::* member)`](./index.md)||
|[`forward(typeTraits::RemoveReference< T > & t)`](./index.md)||
|[`forward(typeTraits::RemoveReference< T > && t)`](./index.md)||
|[`getOut()`](./index.md)||
|[`makeUnexpected(E && e)`](./index.md)||
|[`max(const T & a)`](./index.md)||
|[`max(const T & a, const T & b, const Ts &... n)`](./index.md)||
|[`memcpy(void * dest, const void * src, const SizeType size)`](./index.md)||
|[`memcpy(char * dest, const char * src, const SizeType size)`](./index.md)||
|[`min(const T & a)`](./index.md)||
|[`min(const T & a, const T & b, const Ts &... n)`](./index.md)||
|[`move(T && arg)`](./index.md)||
|[`offsetOf(const M T::* member)`](./index.md)||
|[`panic()`](./index.md)||
|[`swap(T & t1, T & t2)`](./index.md)||
|[`yield()`](./index.md)||

|Typedef||
|:---|:---|
|[`AlignedStorage`](./index.md)|alias of [`bzd::impl::AlignedStorage`](impl/alignedstorage/index.md)|
|[`BoolType`](./index.md)|Boolean type.|
|[`Buffer`](./index.md)|alias of [`bzd::impl::Buffer`](impl/buffer/index.md)|
|[`ConstBuffer`](./index.md)|alias of [`bzd::impl::Buffer`](impl/buffer/index.md)|
|[`Expected`](./index.md)|This is the type used for returning and propagating errors.; alias of [`bzd::impl::Expected`](impl/expected/index.md)|
|[`FctPtrType`](./index.md)|Function pointer type.|
|[`IChannel`](./index.md)|alias of [`bzd::impl::IChannel`](impl/ichannel/index.md)|
|[`Int16Type`](./index.md)|Signed integer type with a fixed width of 16-bit.|
|[`Int32Type`](./index.md)|Signed integer type with a fixed width of 32-bit.|
|[`Int64Type`](./index.md)|Signed integer type with a fixed width of 64-bit.|
|[`Int8Type`](./index.md)|Signed integer type with a fixed width of 8-bit.|
|[`IntPtrType`](./index.md)|Integer type capable of holding a memory pointer.|
|[`IOChannel`](./index.md)|alias of [`bzd::impl::IOChannel`](impl/iochannel/index.md)|
|[`IOStream`](./index.md)|alias of [`bzd::impl::IOStream`](impl/iostream/index.md)|
|[`IStream`](./index.md)|alias of [`bzd::impl::IStream`](impl/istream/index.md)|
|[`OChannel`](./index.md)|alias of [`bzd::impl::OChannel`](impl/ochannel/index.md)|
|[`Optional`](./index.md)|Type managing an optional contained value, i.e. a value that may or may not be present.; alias of [`bzd::impl::Optional`](impl/optional/index.md)|
|[`OStream`](./index.md)|alias of [`bzd::impl::OStream`](impl/ostream/index.md)|
|[`PtrType`](./index.md)|Memory pointer type.|
|[`SizeType`](./index.md)|Unsigned integer that can store the maximum size of a theoretically possible object of any type.|
|[`StringConstexpr`](./index.md)|alias of [`bzd::impl::StringConstexpr`](impl/stringconstexpr/index.md)|
|[`StringView`](./index.md)|alias of [`bzd::impl::StringView`](impl/stringview/index.md)|
|[`UInt16Type`](./index.md)|Unsigned integer type with a fixed width of 16-bit.|
|[`UInt32Type`](./index.md)|Unsigned integer type with a fixed width of 32-bit.|
|[`UInt64Type`](./index.md)|Unsigned integer type with a fixed width of 64-bit.|
|[`UInt8Type`](./index.md)|Unsigned integer type with a fixed width of 8-bit.|
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
### `class Log`

------
### `template<class K, class V, SizeType N> class Map`

#### Template
||||
|---:|:---|:---|
|class|K||
|class|V||
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
### `template<class T, SizeType N, class CapacityType> class Queue`

#### Template
||||
|---:|:---|:---|
|class|T||
|SizeType|N||
|class|CapacityType||
------
### `template<class T> class ReferenceWrapper`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class Interface> class Registry`
Fixed-size registry object.

A registry is a singleton object acting as a key-value store for object instances. The size of the registry must be defined before populating the object, to do so, it is preferable to do it at startup during dynamic variable initialization.

```c++
// Declares a registry which can contain up to 3 doubles
bzd::Registry<double>::Declare<3> registry_;
```

The following snipet shows how to register objects within this registry. Note that no more of N objects (number specified during registry declaration) can be added.

```c++
// Creates and insert 3 objects within the registry
bzd::Registry<double>::Register<> registry_{"A", 42};
bzd::Registry<double>::Register<> registry_{"B", -1};
bzd::Registry<double>::Register<> registry_{"C", 3.14};
```

Note, this initialization scheme can be imediatly followed by object registrations, as order of global variables in a single translation unit (source file) are initialized in the order in which they are defined.
#### Template
||||
|---:|:---|:---|
|class Interface|None||
------
### `class Scheduler`

------
### `template<class Type, bzd::SizeType OffsetBits, bzd::SizeType SizeBits, class CompuMethod> class Signal`

#### Template
||||
|---:|:---|:---|
|class Type|None||
|bzd::SizeType|OffsetBits||
|bzd::SizeType|SizeBits||
|class CompuMethod|None||
------
### `template<class T> class Singleton`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> class SingletonThreadLocal`

#### Template
||||
|---:|:---|:---|
|class|T||
------
### `template<class T> class Span`
The class template span describes an object that can refer to a contiguous sequence of objects with the first element of the sequence at position zero.
#### Template
||||
|---:|:---|:---|
|class|T|Element type, must be a complete type that is not an abstract class type. |
------
### `template<const SizeType N> class Stack`

#### Template
||||
|---:|:---|:---|
|const SizeType|N||
------
### `template<SizeType N> class String`

#### Template
||||
|---:|:---|:---|
|SizeType|N||
------
### `template<SizeType N> class StringStream`

#### Template
||||
|---:|:---|:---|
|SizeType|N||
------
### `template<class T, class... Args> class Task`

#### Template
||||
|---:|:---|:---|
|class T|None||
|class...|Args||
------
### `template<class... T> class Tuple`

#### Template
||||
|---:|:---|:---|
|class...|T||
------
### `template<class... Ts> class Variant`

#### Template
||||
|---:|:---|:---|
|class...|Ts||
------
### `template<class... Ts> class VariantConstexpr`

#### Template
||||
|---:|:---|:---|
|class...|Ts||
------
### `template<class T, SizeType N> class Vector`

#### Template
||||
|---:|:---|:---|
|class|T||
|SizeType|N||
------
### `template<class T> struct resultOf`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T, class... Args> struct resultOf< T(&)(Args &&...)>`

#### Template
||||
|---:|:---|:---|
|class T|None||
|class...|Args||
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
### `template<class T> constexpr T && forward(typeTraits::RemoveReference< T > & t)`

#### Template
||||
|---:|:---|:---|
|class T|None||
#### Parameters
||||
|---:|:---|:---|
|typeTraits::RemoveReference< T > &|t||
------
### `template<class T> constexpr T && forward(typeTraits::RemoveReference< T > && t)`

#### Template
||||
|---:|:---|:---|
|class T|None||
#### Parameters
||||
|---:|:---|:---|
|typeTraits::RemoveReference< T > &&|t||
------
### `bzd::OStream & getOut()`

------
### `template<class E> constexpr impl::Unexpected< bzd::typeTraits::Decay< E > > makeUnexpected(E && e)`

#### Template
||||
|---:|:---|:---|
|class E|None||
#### Parameters
||||
|---:|:---|:---|
|E &&|e||
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
### `constexpr void memcpy(void * dest, const void * src, const SizeType size)`

#### Parameters
||||
|---:|:---|:---|
|void *|dest||
|const void *|src||
|const SizeType|size||
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
### `template<class T> bzd::typeTraits::RemoveReference< T > && move(T && arg)`

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
### `void panic()`

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
### `void yield()`

------
### `template<SizeType Len, SizeType Align> typedef AlignedStorage`

#### Template
||||
|---:|:---|:---|
|SizeType|Len||
|SizeType|Align||
------
### `typedef BoolType`
Boolean type.
------
### `typedef Buffer`

------
### `typedef ConstBuffer`

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
### `typedef FctPtrType`
Function pointer type.
------
### `typedef IChannel`

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
### `typedef Int8Type`
Signed integer type with a fixed width of 8-bit.
------
### `typedef IntPtrType`
Integer type capable of holding a memory pointer.
------
### `typedef IOChannel`

------
### `typedef IOStream`

------
### `typedef IStream`

------
### `typedef OChannel`

------
### `template<class T> typedef Optional`
Type managing an optional contained value, i.e. a value that may or may not be present.
#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `typedef OStream`

------
### `typedef PtrType`
Memory pointer type.
------
### `typedef SizeType`
Unsigned integer that can store the maximum size of a theoretically possible object of any type.
------
### `template<char... C> typedef StringConstexpr`

#### Template
||||
|---:|:---|:---|
|char...|C||
------
### `typedef StringView`

------
### `typedef UInt16Type`
Unsigned integer type with a fixed width of 16-bit.
------
### `typedef UInt32Type`
Unsigned integer type with a fixed width of 32-bit.
------
### `typedef UInt64Type`
Unsigned integer type with a fixed width of 64-bit.
------
### `typedef UInt8Type`
Unsigned integer type with a fixed width of 8-bit.
