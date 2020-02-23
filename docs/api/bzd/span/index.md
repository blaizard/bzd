# [`bzd`](../../index.md)::[`Span`](../index.md)

## `template<class T> class Span`
The class template span describes an object that can refer to a contiguous sequence of objects with the first element of the sequence at position zero.
#### Template
||||
|---:|:---|:---|
|class|T|Element type, must be a complete type that is not an abstract class type. |

|Function||
|:---|:---|
|[`Span()`](./index.md)||
|[`Span(DataType *const data, const SizeType size)`](./index.md)||
|[`Span(const Span< bzd::typeTraits::RemoveConst< DataType >> & span)`](./index.md)||
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
### `constexpr Span()`

------
### `constexpr Span(DataType *const data, const SizeType size)`

#### Parameters
||||
|---:|:---|:---|
|DataType *const|data||
|const SizeType|size||
------
### `template<class Q, bzd::typeTraits::EnableIf< Q::value, void > *> constexpr Span(const Span< bzd::typeTraits::RemoveConst< DataType >> & span)`

#### Template
||||
|---:|:---|:---|
|class Q|None||
|bzd::typeTraits::EnableIf< Q::value, void > *|None||
#### Parameters
||||
|---:|:---|:---|
|const Span< bzd::typeTraits::RemoveConst< DataType >> &|span||
------
### `template<class Q, bzd::typeTraits::EnableIf<!Q::value, void > *> constexpr DataType & at(const SizeType index)`

#### Template
||||
|---:|:---|:---|
|class Q|None||
|bzd::typeTraits::EnableIf<!Q::value, void > *|None||
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
### `template<class Q, bzd::typeTraits::EnableIf<!Q::value, void > *> constexpr DataType & back()`

#### Template
||||
|---:|:---|:---|
|class Q|None||
|bzd::typeTraits::EnableIf<!Q::value, void > *|None||
------
### `constexpr const DataType & back() const`

------
### `template<class Q, bzd::typeTraits::EnableIf<!Q::value, void > *> constexpr Iterator begin()`

#### Template
||||
|---:|:---|:---|
|class Q|None||
|bzd::typeTraits::EnableIf<!Q::value, void > *|None||
------
### `constexpr ConstIterator begin() const`

------
### `constexpr ConstIterator cbegin() const`

------
### `constexpr ConstIterator cend() const`

------
### `template<class Q, bzd::typeTraits::EnableIf<!Q::value, void > *> constexpr DataType * data()`

#### Template
||||
|---:|:---|:---|
|class Q|None||
|bzd::typeTraits::EnableIf<!Q::value, void > *|None||
------
### `constexpr const DataType * data() const`

------
### `constexpr bool empty() const`

------
### `template<class Q, bzd::typeTraits::EnableIf<!Q::value, void > *> constexpr Iterator end()`

#### Template
||||
|---:|:---|:---|
|class Q|None||
|bzd::typeTraits::EnableIf<!Q::value, void > *|None||
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
### `template<class Q, bzd::typeTraits::EnableIf<!Q::value, void > *> constexpr DataType & front()`

#### Template
||||
|---:|:---|:---|
|class Q|None||
|bzd::typeTraits::EnableIf<!Q::value, void > *|None||
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
### `template<class Q, bzd::typeTraits::EnableIf<!Q::value, void > *> constexpr DataType & operator[](const SizeType index)`

#### Template
||||
|---:|:---|:---|
|class Q|None||
|bzd::typeTraits::EnableIf<!Q::value, void > *|None||
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

