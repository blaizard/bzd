# [`bzd`](../../../index.md)::[`impl`](../../index.md)::[`Pool`](../index.md)

## `template<class T, class CapacityType> class Pool`

#### Template
||||
|---:|:---|:---|
|class|T||
|class|CapacityType||

|Function||
|:---|:---|
|[`Pool(const bzd::Span< Element > data)`](./index.md)||
|[`release(T & container)`](./index.md)||
|[`reserve()`](./index.md)||
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
|[`operator==(const SelfType & rhs) const`](./index.md)||
|[`operator!=(const SelfType & rhs) const`](./index.md)||
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
|[`operator==(const SelfType & rhs) const`](./index.md)||
|[`operator!=(const SelfType & rhs) const`](./index.md)||
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
|[`Iterator`](./index.md)|alias of [`bzd::iterator::Contiguous`](../../iterator/contiguous/index.md)|
|[`ConstIterator`](./index.md)|alias of [`bzd::iterator::Contiguous`](../../iterator/contiguous/index.md)|

|Variable||
|:---|:---|
|[`npos`](./index.md)||
|[`npos`](./index.md)||
|[`npos`](./index.md)||
------
### `explicit constexpr Pool(const bzd::Span< Element > data)`

#### Parameters
||||
|---:|:---|:---|
|const bzd::Span< Element >|data||
------
### `constexpr void release(T & container)`
Release an element from the pool
#### Parameters
||||
|---:|:---|:---|
|T &|container||
------
### `constexpr T & reserve()`
Reserve an element from the free list (if any)
------
### `explicit constexpr SingleLinkedPool(const bzd::Span< Element > data)`
*From bzd::impl::SingleLinkedPool*


#### Parameters
||||
|---:|:---|:---|
|const bzd::Span< Element >|data||
------
### `constexpr CapacityType capacity() const`
*From bzd::impl::SingleLinkedPool*


------
### `constexpr bool empty() const`
*From bzd::impl::SingleLinkedPool*


------
### `constexpr void release(Element & element)`
*From bzd::impl::SingleLinkedPool*

Release an element from the pool
#### Parameters
||||
|---:|:---|:---|
|Element &|element||
------
### `constexpr Element & reserve()`
*From bzd::impl::SingleLinkedPool*

Reserve an element from the free list (if any)
------
### `constexpr CapacityType getIndex(const Element & element) const`
*From bzd::impl::SingleLinkedPool*


#### Parameters
||||
|---:|:---|:---|
|const Element &|element||
------
### `void toStream(std::ostream & os)`
*From bzd::impl::SingleLinkedPool*


#### Parameters
||||
|---:|:---|:---|
|std::ostream &|os||
------
### `constexpr Span(DataType *const data, const SizeType size)`
*From bzd::impl::SingleLinkedPool*


#### Parameters
||||
|---:|:---|:---|
|DataType *const|data||
|const SizeType|size||
------
### `template<class Q, typename bzd::typeTraits::enableIf< Q::value, void >::type *> constexpr Span(const Span< typename bzd::typeTraits::removeConst< DataType >::type > & span)`
*From bzd::impl::SingleLinkedPool*


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
*From bzd::impl::SingleLinkedPool*


#### Template
||||
|---:|:---|:---|
|class Q|None||
|typename bzd::typeTraits::enableIf<!Q::value, void >::type *|None||
------
### `template<class Q, typename bzd::typeTraits::enableIf<!Q::value, void >::type *> constexpr Iterator end()`
*From bzd::impl::SingleLinkedPool*


#### Template
||||
|---:|:---|:---|
|class Q|None||
|typename bzd::typeTraits::enableIf<!Q::value, void >::type *|None||
------
### `constexpr ConstIterator begin() const`
*From bzd::impl::SingleLinkedPool*


------
### `constexpr ConstIterator cbegin() const`
*From bzd::impl::SingleLinkedPool*


------
### `constexpr ConstIterator end() const`
*From bzd::impl::SingleLinkedPool*


------
### `constexpr ConstIterator cend() const`
*From bzd::impl::SingleLinkedPool*


------
### `constexpr SizeType size() const`
*From bzd::impl::SingleLinkedPool*


------
### `constexpr void reverse()`
*From bzd::impl::SingleLinkedPool*


------
### `constexpr bool operator==(const SelfType & rhs) const`
*From bzd::impl::SingleLinkedPool*


#### Parameters
||||
|---:|:---|:---|
|const SelfType &|rhs||
------
### `constexpr bool operator!=(const SelfType & rhs) const`
*From bzd::impl::SingleLinkedPool*


#### Parameters
||||
|---:|:---|:---|
|const SelfType &|rhs||
------
### `template<class Q, typename bzd::typeTraits::enableIf<!Q::value, void >::type *> constexpr DataType & operator[](const SizeType index)`
*From bzd::impl::SingleLinkedPool*


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
*From bzd::impl::SingleLinkedPool*


#### Parameters
||||
|---:|:---|:---|
|const SizeType|index||
------
### `template<class Q, typename bzd::typeTraits::enableIf<!Q::value, void >::type *> constexpr DataType & at(const SizeType index)`
*From bzd::impl::SingleLinkedPool*


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
*From bzd::impl::SingleLinkedPool*


#### Parameters
||||
|---:|:---|:---|
|const SizeType|index||
------
### `template<class Q, typename bzd::typeTraits::enableIf<!Q::value, void >::type *> constexpr DataType & front()`
*From bzd::impl::SingleLinkedPool*


#### Template
||||
|---:|:---|:---|
|class Q|None||
|typename bzd::typeTraits::enableIf<!Q::value, void >::type *|None||
------
### `constexpr const DataType & front() const`
*From bzd::impl::SingleLinkedPool*


------
### `template<class Q, typename bzd::typeTraits::enableIf<!Q::value, void >::type *> constexpr DataType & back()`
*From bzd::impl::SingleLinkedPool*


#### Template
||||
|---:|:---|:---|
|class Q|None||
|typename bzd::typeTraits::enableIf<!Q::value, void >::type *|None||
------
### `constexpr const DataType & back() const`
*From bzd::impl::SingleLinkedPool*


------
### `template<class Q, typename bzd::typeTraits::enableIf<!Q::value, void >::type *> constexpr DataType * data()`
*From bzd::impl::SingleLinkedPool*


#### Template
||||
|---:|:---|:---|
|class Q|None||
|typename bzd::typeTraits::enableIf<!Q::value, void >::type *|None||
------
### `constexpr const DataType * data() const`
*From bzd::impl::SingleLinkedPool*


------
### `constexpr SizeType find(const DataType & item, const SizeType start) const`
*From bzd::impl::SingleLinkedPool*


#### Parameters
||||
|---:|:---|:---|
|const DataType &|item||
|const SizeType|start||
------
### `constexpr bool empty() const`
*From bzd::impl::SingleLinkedPool*


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
### `constexpr bool operator==(const SelfType & rhs) const`
*From bzd::Span*


#### Parameters
||||
|---:|:---|:---|
|const SelfType &|rhs||
------
### `constexpr bool operator!=(const SelfType & rhs) const`
*From bzd::Span*


#### Parameters
||||
|---:|:---|:---|
|const SelfType &|rhs||
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
*From bzd::impl::SingleLinkedPool*


------
### `typedef Parent`
*From bzd::impl::SingleLinkedPool*


------
### `typedef Iterator`
*From bzd::impl::SingleLinkedPool*


------
### `typedef ConstIterator`
*From bzd::impl::SingleLinkedPool*


------
### `typedef Iterator`
*From bzd::Span*


------
### `typedef ConstIterator`
*From bzd::Span*


------
### `static constexpr const CapacityType npos`
*From bzd::impl::SingleLinkedPool*


------
### `static constexpr const SizeType npos`
*From bzd::impl::SingleLinkedPool*


------
### `static constexpr const SizeType npos`
*From bzd::Span*


