# [`bzd`](../../index.md)::[`Array`](../index.md)

## `template<class T, SizeType N> class Array`
A container that encapsulates fixed size arrays.

The container combines the performance and accessibility of a C-style array with the benefits of a standard container, such as knowing its own size, supporting assignment, random access iterators, etc. Unlike a C-style array, it doesn't decay to T* automatically.
#### Template
||||
|---:|:---|:---|
|class|T||
|SizeType|N||

|Function||
|:---|:---|
|[`Array(Args &&... args)`](./index.md)||
|[`capacity() const`](./index.md)|Returns the number of elements that the array can hold.|
|[`Span(DataType *const data, const SizeType size)`](./index.md)||
|[`Span(const Span< typename bzd::typeTraits::removeConst< DataType >::type > & span)`](./index.md)||
|[`begin()`](./index.md)||
|[`end()`](./index.md)||
|[`begin() const`](./index.md)||
|[`cbegin() const`](./index.md)||
|[`end() const`](./index.md)||
|[`cend() const`](./index.md)||
|[`size() const`](./index.md)||
|[`reverse()`](./index.md)||
|[`operator[](const SizeType index)`](./index.md)||
|[`operator[](const SizeType index) const`](./index.md)||
|[`at(const SizeType index)`](./index.md)||
|[`at(const SizeType index) const`](./index.md)||
|[`front()`](./index.md)||
|[`front() const`](./index.md)||
|[`back()`](./index.md)||
|[`back() const`](./index.md)||
|[`data()`](./index.md)||
|[`data() const`](./index.md)||
|[`find(const DataType & item, const SizeType start) const`](./index.md)||
|[`empty() const`](./index.md)||

|Typedef||
|:---|:---|
|[`Iterator`](./index.md)|alias of [`bzd::iterator::Contiguous`](../iterator/contiguous/index.md)|
|[`ConstIterator`](./index.md)|alias of [`bzd::iterator::Contiguous`](../iterator/contiguous/index.md)|

|Variable||
|:---|:---|
|[`npos`](./index.md)||
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


