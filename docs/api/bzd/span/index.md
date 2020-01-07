# [`bzd`](../../index.md)::[`Span`](../index.md)

## `template<class T> class Span`
The class template span describes an object that can refer to a contiguous sequence of objects with the first element of the sequence at position zero.
#### Template
||||
|---:|:---|:---|
|class|T|Element type, must be a complete type that is not an abstract class type. |

|Function||
|:---|:---|
|[`Span(DataType *const data, const SizeType size)`](./index.md)||
|[`Span(const Span< typename bzd::typeTraits::removeConst< DataType >::type > & span)`](./index.md)||
|[`at(const SizeType index)`](./index.md)||
|[`at(const SizeType index) const`](./index.md)||
|[`back()`](./index.md)||
|[`back() const`](./index.md)||
|[`begin()`](./index.md)||
|[`begin() const`](./index.md)||
|[`cbegin() const`](./index.md)||
|[`cend() const`](./index.md)||
|[`data()`](./index.md)||
|[`data() const`](./index.md)||
|[`empty() const`](./index.md)||
|[`end()`](./index.md)||
|[`end() const`](./index.md)||
|[`find(const DataType & item, const SizeType start) const`](./index.md)||
|[`front()`](./index.md)||
|[`front() const`](./index.md)||
|[`operator!=(const SelfType & rhs) const`](./index.md)||
|[`operator==(const SelfType & rhs) const`](./index.md)||
|[`operator[](const SizeType index)`](./index.md)||
|[`operator[](const SizeType index) const`](./index.md)||
|[`reverse()`](./index.md)||
|[`size() const`](./index.md)||

|Typedef||
|:---|:---|
|[`ConstIterator`](./index.md)|alias of [`bzd::iterator::Contiguous`](../iterator/contiguous/index.md)|
|[`Iterator`](./index.md)|alias of [`bzd::iterator::Contiguous`](../iterator/contiguous/index.md)|

|Variable||
|:---|:---|
|[`npos`](./index.md)||
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
### `template<class Q, typename bzd::typeTraits::enableIf<!Q::value, void >::type *> constexpr DataType & back()`

#### Template
||||
|---:|:---|:---|
|class Q|None||
|typename bzd::typeTraits::enableIf<!Q::value, void >::type *|None||
------
### `constexpr const DataType & back() const`

------
### `template<class Q, typename bzd::typeTraits::enableIf<!Q::value, void >::type *> constexpr Iterator begin()`

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
### `constexpr ConstIterator cend() const`

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
### `constexpr bool empty() const`

------
### `template<class Q, typename bzd::typeTraits::enableIf<!Q::value, void >::type *> constexpr Iterator end()`

#### Template
||||
|---:|:---|:---|
|class Q|None||
|typename bzd::typeTraits::enableIf<!Q::value, void >::type *|None||
------
### `constexpr ConstIterator end() const`

------
### `constexpr SizeType find(const DataType & item, const SizeType start) const`

#### Parameters
||||
|---:|:---|:---|
|const DataType &|item||
|const SizeType|start||
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
### `constexpr bool operator!=(const SelfType & rhs) const`

#### Parameters
||||
|---:|:---|:---|
|const SelfType &|rhs||
------
### `constexpr bool operator==(const SelfType & rhs) const`

#### Parameters
||||
|---:|:---|:---|
|const SelfType &|rhs||
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
### `constexpr void reverse()`

------
### `constexpr SizeType size() const`

------
### `typedef ConstIterator`

------
### `typedef Iterator`

------
### `static constexpr const SizeType npos`

