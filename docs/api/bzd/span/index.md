# [`bzd`](../../index.md)::[`Span`](../index.md)

## `template<class T> class Span`
The class template span describes an object that can refer to a contiguous sequence of objects with the first element of the sequence at position zero.
#### Template
||||
|---:|:---|:---|
|class|T|Element type, must be a complete type that is not an abstract class type. |
### Function
||||
|---:|:---|:---|
|constexpr|[`Span(DataType *const data, const SizeType size)`](./index.md)||
|constexpr|[`Span(const Span< typename bzd::typeTraits::removeConst< DataType >::type > & span)`](./index.md)||
|constexpr Iterator|[`begin()`](./index.md)||
|constexpr Iterator|[`end()`](./index.md)||
|constexpr ConstIterator|[`begin() const`](./index.md)||
|constexpr ConstIterator|[`cbegin() const`](./index.md)||
|constexpr ConstIterator|[`end() const`](./index.md)||
|constexpr ConstIterator|[`cend() const`](./index.md)||
|constexpr SizeType|[`size() const`](./index.md)||
|constexpr void|[`reverse()`](./index.md)||
|constexpr DataType &|[`operator[](const SizeType index)`](./index.md)||
|constexpr const DataType &|[`operator[](const SizeType index) const`](./index.md)||
|constexpr DataType &|[`at(const SizeType index)`](./index.md)||
|constexpr const T &|[`at(const SizeType index) const`](./index.md)||
|constexpr DataType &|[`front()`](./index.md)||
|constexpr const DataType &|[`front() const`](./index.md)||
|constexpr DataType &|[`back()`](./index.md)||
|constexpr const DataType &|[`back() const`](./index.md)||
|constexpr DataType *|[`data()`](./index.md)||
|constexpr const DataType *|[`data() const`](./index.md)||
|constexpr SizeType|[`find(const DataType & item, const SizeType start) const`](./index.md)||
|constexpr bool|[`empty() const`](./index.md)||
### Typedef
||||
|---:|:---|:---|
|typedef|[`Iterator`](./index.md)|alias of [`bzd::iterator::Contiguous`](../iterator/contiguous/index.md)|
|typedef|[`ConstIterator`](./index.md)|alias of [`bzd::iterator::Contiguous`](../iterator/contiguous/index.md)|
### Variable
||||
|---:|:---|:---|
|constexpr const SizeType|[`npos`](./index.md)||
------
### `constexpr Span(DataType *const data, const SizeType size)`

#### Parameters
||||
|---:|:---|:---|
|DataType *const|data||
|const SizeType|size||
------
### `template<class Q, typename bzd::typeTraits::enableIf< Q::value, void >::type *> constexpr Span(const Span< typename bzd::typeTraits::removeConst< DataType >::type > & span)`

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

#### Template
||||
|---:|:---|:---|
|class Q|None||
|typename bzd::typeTraits::enableIf<!Q::value, void >::type *|None||
------
### `template<class Q, typename bzd::typeTraits::enableIf<!Q::value, void >::type *> constexpr Iterator end()`

#### Template
||||
|---:|:---|:---|
|class Q|None||
|typename bzd::typeTraits::enableIf<!Q::value, void >::type *|None||
------
### `constexpr ConstIterator begin() const`

------
### `constexpr ConstIterator cbegin() const`

------
### `constexpr ConstIterator end() const`

------
### `constexpr ConstIterator cend() const`

------
### `constexpr SizeType size() const`

------
### `constexpr void reverse()`

------
### `template<class Q, typename bzd::typeTraits::enableIf<!Q::value, void >::type *> constexpr DataType & operator[](const SizeType index)`

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

#### Parameters
||||
|---:|:---|:---|
|const SizeType|index||
------
### `template<class Q, typename bzd::typeTraits::enableIf<!Q::value, void >::type *> constexpr DataType & at(const SizeType index)`

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

#### Parameters
||||
|---:|:---|:---|
|const SizeType|index||
------
### `template<class Q, typename bzd::typeTraits::enableIf<!Q::value, void >::type *> constexpr DataType & front()`

#### Template
||||
|---:|:---|:---|
|class Q|None||
|typename bzd::typeTraits::enableIf<!Q::value, void >::type *|None||
------
### `constexpr const DataType & front() const`

------
### `template<class Q, typename bzd::typeTraits::enableIf<!Q::value, void >::type *> constexpr DataType & back()`

#### Template
||||
|---:|:---|:---|
|class Q|None||
|typename bzd::typeTraits::enableIf<!Q::value, void >::type *|None||
------
### `constexpr const DataType & back() const`

------
### `template<class Q, typename bzd::typeTraits::enableIf<!Q::value, void >::type *> constexpr DataType * data()`

#### Template
||||
|---:|:---|:---|
|class Q|None||
|typename bzd::typeTraits::enableIf<!Q::value, void >::type *|None||
------
### `constexpr const DataType * data() const`

------
### `constexpr SizeType find(const DataType & item, const SizeType start) const`

#### Parameters
||||
|---:|:---|:---|
|const DataType &|item||
|const SizeType|start||
------
### `constexpr bool empty() const`

------
### `typedef Iterator`

------
### `typedef ConstIterator`

------
### `static constexpr const SizeType npos`

