# [`bzd`](../../index.md)::[`Array`](../index.md)

## `template<class T, SizeType N> class Array`
A container that encapsulates fixed size arrays.

The container combines the performance and accessibility of a C-style array with the benefits of a standard container, such as knowing its own size, supporting assignment, random access iterators, etc. Unlike a C-style array, it doesn't decay to T* automatically.
#### Template
||||
|---:|:---|:---|
|class|T||
|SizeType|N||
### Function
||||
|---:|:---|:---|
|constexpr|[`Array(Args &&... args)`](.)||
|constexpr SizeType|[`capacity() const`](.)|Returns the number of elements that the array can hold.|
|constexpr|[`Span(DataType *const data, const SizeType size)`](.)||
|constexpr|[`Span(const Span< typename bzd::typeTraits::removeConst< DataType >::type > & span)`](.)||
|constexpr Iterator|[`begin()`](.)||
|constexpr Iterator|[`end()`](.)||
|constexpr ConstIterator|[`begin() const`](.)||
|constexpr ConstIterator|[`cbegin() const`](.)||
|constexpr ConstIterator|[`end() const`](.)||
|constexpr ConstIterator|[`cend() const`](.)||
|constexpr SizeType|[`size() const`](.)||
|constexpr void|[`reverse()`](.)||
|constexpr DataType &|[`operator[](const SizeType index)`](.)||
|constexpr const DataType &|[`operator[](const SizeType index) const`](.)||
|constexpr DataType &|[`at(const SizeType index)`](.)||
|constexpr const T &|[`at(const SizeType index) const`](.)||
|constexpr DataType &|[`front()`](.)||
|constexpr const DataType &|[`front() const`](.)||
|constexpr DataType &|[`back()`](.)||
|constexpr const DataType &|[`back() const`](.)||
|constexpr DataType *|[`data()`](.)||
|constexpr const DataType *|[`data() const`](.)||
|constexpr SizeType|[`find(const DataType & item, const SizeType start) const`](.)||
|constexpr bool|[`empty() const`](.)||
### Typedef
||||
|---:|:---|:---|
|typedef|[`Iterator`](.)|alias of [`bzd::iterator::Contiguous`](../iterator/contiguous/index.md)|
|typedef|[`ConstIterator`](.)|alias of [`bzd::iterator::Contiguous`](../iterator/contiguous/index.md)|
### Variable
||||
|---:|:---|:---|
|constexpr const SizeType|[`npos`](.)||
------
### `template<class... Args> explicit constexpr Array(Args &&... args)`

#### Template
||||
|---:|:---|:---|
|class...|Args||
#### Parameters
||||
|---:|:---|:---|
|Args &&...|args||
------
### `constexpr SizeType capacity() const`
Returns the number of elements that the array can hold.

Maximum number of element this array can hold.
------
### `constexpr Span(DataType *const data, const SizeType size)`
*From bzd::Span*


#### Parameters
||||
|---:|:---|:---|
|DataType *const|data||
|const SizeType|size||
------
### `template<class Q, typename bzd::typeTraits::enableIf< Q::value, void >::type *> constexpr Span(const Span< typename bzd::typeTraits::removeConst< DataType >::type > & span)`
*From bzd::Span*


#### Template
||||
|---:|:---|:---|
|class Q|None||
|typename bzd::typeTraits::enableIf< Q::value, void >::type *|None||
#### Parameters
||||
|---:|:---|:---|
|const Span< typename bzd::typeTraits::removeConst< DataType >::type > &|span||
------
### `template<class Q, typename bzd::typeTraits::enableIf<!Q::value, void >::type *> constexpr Iterator begin()`
*From bzd::Span*


#### Template
||||
|---:|:---|:---|
|class Q|None||
|typename bzd::typeTraits::enableIf<!Q::value, void >::type *|None||
------
### `template<class Q, typename bzd::typeTraits::enableIf<!Q::value, void >::type *> constexpr Iterator end()`
*From bzd::Span*


#### Template
||||
|---:|:---|:---|
|class Q|None||
|typename bzd::typeTraits::enableIf<!Q::value, void >::type *|None||
------
### `constexpr ConstIterator begin() const`
*From bzd::Span*


------
### `constexpr ConstIterator cbegin() const`
*From bzd::Span*


------
### `constexpr ConstIterator end() const`
*From bzd::Span*


------
### `constexpr ConstIterator cend() const`
*From bzd::Span*


------
### `constexpr SizeType size() const`
*From bzd::Span*


------
### `constexpr void reverse()`
*From bzd::Span*


------
### `template<class Q, typename bzd::typeTraits::enableIf<!Q::value, void >::type *> constexpr DataType & operator[](const SizeType index)`
*From bzd::Span*


#### Template
||||
|---:|:---|:---|
|class Q|None||
|typename bzd::typeTraits::enableIf<!Q::value, void >::type *|None||
#### Parameters
||||
|---:|:---|:---|
|const SizeType|index||
------
### `constexpr const DataType & operator[](const SizeType index) const`
*From bzd::Span*


#### Parameters
||||
|---:|:---|:---|
|const SizeType|index||
------
### `template<class Q, typename bzd::typeTraits::enableIf<!Q::value, void >::type *> constexpr DataType & at(const SizeType index)`
*From bzd::Span*


#### Template
||||
|---:|:---|:---|
|class Q|None||
|typename bzd::typeTraits::enableIf<!Q::value, void >::type *|None||
#### Parameters
||||
|---:|:---|:---|
|const SizeType|index||
------
### `constexpr const T & at(const SizeType index) const`
*From bzd::Span*


#### Parameters
||||
|---:|:---|:---|
|const SizeType|index||
------
### `template<class Q, typename bzd::typeTraits::enableIf<!Q::value, void >::type *> constexpr DataType & front()`
*From bzd::Span*


#### Template
||||
|---:|:---|:---|
|class Q|None||
|typename bzd::typeTraits::enableIf<!Q::value, void >::type *|None||
------
### `constexpr const DataType & front() const`
*From bzd::Span*


------
### `template<class Q, typename bzd::typeTraits::enableIf<!Q::value, void >::type *> constexpr DataType & back()`
*From bzd::Span*


#### Template
||||
|---:|:---|:---|
|class Q|None||
|typename bzd::typeTraits::enableIf<!Q::value, void >::type *|None||
------
### `constexpr const DataType & back() const`
*From bzd::Span*


------
### `template<class Q, typename bzd::typeTraits::enableIf<!Q::value, void >::type *> constexpr DataType * data()`
*From bzd::Span*


#### Template
||||
|---:|:---|:---|
|class Q|None||
|typename bzd::typeTraits::enableIf<!Q::value, void >::type *|None||
------
### `constexpr const DataType * data() const`
*From bzd::Span*


------
### `constexpr SizeType find(const DataType & item, const SizeType start) const`
*From bzd::Span*


#### Parameters
||||
|---:|:---|:---|
|const DataType &|item||
|const SizeType|start||
------
### `constexpr bool empty() const`
*From bzd::Span*


------
### `typedef Iterator`
*From bzd::Span*


------
### `typedef ConstIterator`
*From bzd::Span*


------
### `static constexpr const SizeType npos`
*From bzd::Span*


