# [`bzd`](../../../index.md)::[`impl`](../../index.md)::[`SingleLinkedPool`](../index.md)

## `template<class T, class CapacityType> class SingleLinkedPool`

#### Template
||||
|---:|:---|:---|
|class|T||
|class|CapacityType||

|Function||
|:---|:---|
|[`SingleLinkedPool(const bzd::Span< Element > data)`](./index.md)||
|[`capacity() const`](./index.md)||
|[`empty() const`](./index.md)||
|[`release(Element & element)`](./index.md)||
|[`reserve()`](./index.md)||
|[`getIndex(const Element & element) const`](./index.md)||
|[`toStream(std::ostream & os)`](./index.md)||
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
|[`Element`](./index.md)|alias of [`bzd::impl::SingleLinkedPoolElement`](../singlelinkedpoolelement/index.md)|
|[`Parent`](./index.md)|alias of [`bzd::Span`](../../span/index.md)|
|[`Iterator`](./index.md)|alias of [`bzd::iterator::Contiguous`](../../iterator/contiguous/index.md)|
|[`ConstIterator`](./index.md)|alias of [`bzd::iterator::Contiguous`](../../iterator/contiguous/index.md)|

|Variable||
|:---|:---|
|[`npos`](./index.md)||
|[`npos`](./index.md)||
------
### `explicit constexpr SingleLinkedPool(const bzd::Span< Element > data)`

#### Parameters
||||
|---:|:---|:---|
|const bzd::Span< Element >|data||
------
### `constexpr CapacityType capacity() const`

------
### `constexpr bool empty() const`

------
### `constexpr void release(Element & element)`
Release an element from the pool
#### Parameters
||||
|---:|:---|:---|
|Element &|element||
------
### `constexpr Element & reserve()`
Reserve an element from the free list (if any)
------
### `constexpr CapacityType getIndex(const Element & element) const`

#### Parameters
||||
|---:|:---|:---|
|const Element &|element||
------
### `void toStream(std::ostream & os)`

#### Parameters
||||
|---:|:---|:---|
|std::ostream &|os||
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
### `typedef Element`

------
### `typedef Parent`

------
### `typedef Iterator`
*From bzd::Span*


------
### `typedef ConstIterator`
*From bzd::Span*


------
### `static constexpr const CapacityType npos`

------
### `static constexpr const SizeType npos`
*From bzd::Span*


