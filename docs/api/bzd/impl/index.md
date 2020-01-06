# [`bzd`](../../index.md)::[`impl`](../index.md)

## ` impl`


|Class||
|:---|:---|
|[`Map`](map/index.md)|Flat map implementation.|
|[`IOStream`](iostream/index.md)||
|[`Optional`](optional/index.md)||
|[`Variant`](variant/index.md)||
|[`ConstexprStringView`](constexprstringview/index.md)||
|[`Vector`](vector/index.md)||
|[`SingleLinkedPool`](singlelinkedpool/index.md)||
|[`Expected`](expected/index.md)||
|[`IStream`](istream/index.md)||
|[`Expected< void, E >`](expected_void_e_/index.md)||
|[`TupleImpl`](tupleimpl/index.md)||
|[`Queue`](queue/index.md)||
|[`StringView`](stringview/index.md)||
|[`BTree`](btree/index.md)||
|[`Unexpected`](unexpected/index.md)||
|[`String`](string/index.md)||
|[`Pool`](pool/index.md)||
|[`TupleElem`](tupleelem/index.md)||
|[`Registry`](registry/index.md)||
|[`OStream`](ostream/index.md)||
|[`TupleImpl< TupleSizes< N... >, T... >`](tupleimpl_tuplesizes_n_t_/index.md)||
|[`StringStream`](stringstream/index.md)||

|Struct||
|:---|:---|
|[`TupleRangeImpl`](tuplerangeimpl/index.md)||
|[`NoType`](notype/index.md)||
|[`SingleLinkedPoolElement`](singlelinkedpoolelement/index.md)||
|[`TupleSizes`](tuplesizes/index.md)||
|[`TupleRangeImpl< L, L, TupleSizes< N... > >`](tuplerangeimpl_l_l_tuplesizes_n_/index.md)||
|[`TupleRangeImpl< L, I, TupleSizes< N... > >`](tuplerangeimpl_l_i_tuplesizes_n_/index.md)||

|Function||
|:---|:---|
|[`tygrab(char const (&) c)`](./index.md)||
|[`typoke(ConstexprStringView< X... >)`](./index.md)||
|[`typoke(ConstexprStringView< X... >, ConstexprStringView<'\0'>, ConstexprStringView< Y > ...)`](./index.md)||
|[`typoke(ConstexprStringView< X... >, ConstexprStringView< A >, ConstexprStringView< Y > ...)`](./index.md)||
|[`typeek(ConstexprStringView< C... >)`](./index.md)||
|[`TupleChooseN()`](./index.md)||
|[`TupleChooseN(T && t, Ts &&... ts)`](./index.md)||
|[`TupleChooseN(T && t, Ts &&... ts)`](./index.md)||
|[`contextSwitch(void ** stack1, void * stack2)`](./index.md)||
|[`contextTask()`](./index.md)||

|Typedef||
|:---|:---|
|[`TupleTypeOf`](./index.md)||
|[`TupleRange`](./index.md)|alias of [`bzd::impl::TupleRangeImpl`](tuplerangeimpl/index.md)|
------
### `template<class K, class V> class Map`
Flat map implementation.
#### Template
||||
|---:|:---|:---|
|class K|None||
|class V|None||
------
### `template<class T> class IOStream`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> class Optional`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class StorageType, class... Ts> class Variant`

#### Template
||||
|---:|:---|:---|
|class|StorageType||
|class...|Ts||
------
### `template<char... C> class ConstexprStringView`

#### Template
||||
|---:|:---|:---|
|char...|C||
------
### `template<class T, class Impl> class Vector`

#### Template
||||
|---:|:---|:---|
|class|T||
|class|Impl||
------
### `template<class T, class CapacityType> class SingleLinkedPool`

#### Template
||||
|---:|:---|:---|
|class|T||
|class|CapacityType||
------
### `template<class T, class E> class Expected`

#### Template
||||
|---:|:---|:---|
|class|T||
|class|E||
------
### `template<class T> class IStream`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class E> class Expected< void, E >`

#### Template
||||
|---:|:---|:---|
|class E|None||
------
### `template<class N, class... T> class TupleImpl`

#### Template
||||
|---:|:---|:---|
|class N|None||
|class...|T||
------
### `template<class T, class CapacityType> class Queue`

#### Template
||||
|---:|:---|:---|
|class|T||
|class|CapacityType||
------
### `template<class T, class Impl> class StringView`

#### Template
||||
|---:|:---|:---|
|class T|None||
|class Impl|None||
------
### `template<class K, class V, SizeType Order> class BTree`
Implementation of a B-tree.

A B-Tree of order M has the following properties:Every nodes has at most M children.A non-leaf node with k children contains k-1 keys.The root has at least 2 children if it is not a leaf node.Every non-leaf node (except root) has at least M / 2 children.All leaves appear in the same level.

Non-leaf nodes are implemented as non-sorted arrays.
#### Template
||||
|---:|:---|:---|
|class K|None||
|class V|None||
|SizeType|Order||
------
### `template<class E> class Unexpected`
Internal class used to create an unexpected object type.
#### Template
||||
|---:|:---|:---|
|class E|None||
------
### `template<class T, class Impl> class String`

#### Template
||||
|---:|:---|:---|
|class T|None||
|class Impl|None||
------
### `template<class T, class CapacityType> class Pool`

#### Template
||||
|---:|:---|:---|
|class|T||
|class|CapacityType||
------
### `template<SizeType N, class T> class TupleElem`

#### Template
||||
|---:|:---|:---|
|SizeType|N||
|class T|None||
------
### `template<class T> class Registry`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> class OStream`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<SizeType... N, class... T> class TupleImpl< TupleSizes< N... >, T... >`

#### Template
||||
|---:|:---|:---|
|SizeType...|N||
|class...|T||
------
### `class StringStream`

------
### `template<SizeType L, SizeType I, class S> struct TupleRangeImpl`

#### Template
||||
|---:|:---|:---|
|SizeType|L||
|SizeType|I||
|class S|None||
------
### `struct NoType`

------
### `template<class T, class CapacityType> struct SingleLinkedPoolElement`

#### Template
||||
|---:|:---|:---|
|class|T||
|class|CapacityType||
------
### `template<SizeType... N> struct TupleSizes`

#### Template
||||
|---:|:---|:---|
|SizeType...|N||
------
### `template<SizeType L, SizeType... N> struct TupleRangeImpl< L, L, TupleSizes< N... > >`

#### Template
||||
|---:|:---|:---|
|SizeType|L||
|SizeType...|N||
------
### `template<SizeType L, SizeType I, SizeType... N> struct TupleRangeImpl< L, I, TupleSizes< N... > >`

#### Template
||||
|---:|:---|:---|
|SizeType|L||
|SizeType|I||
|SizeType...|N||
------
### `template<int N, int M> constexpr char tygrab(char const (&) c)`

#### Template
||||
|---:|:---|:---|
|int|N||
|int|M||
#### Parameters
||||
|---:|:---|:---|
|char const (&)|c||
------
### `template<char... X> auto typoke(ConstexprStringView< X... >)`

#### Template
||||
|---:|:---|:---|
|char...|X||
#### Parameters
||||
|---:|:---|:---|
|ConstexprStringView< X... >|None||
------
### `template<char... X, char... Y> auto typoke(ConstexprStringView< X... >, ConstexprStringView<'\0'>, ConstexprStringView< Y > ...)`

#### Template
||||
|---:|:---|:---|
|char...|X||
|char...|Y||
#### Parameters
||||
|---:|:---|:---|
|ConstexprStringView< X... >|None||
|ConstexprStringView<'\0'>|None||
|ConstexprStringView< Y >|...||
------
### `template<char A, char... X, char... Y> auto typoke(ConstexprStringView< X... >, ConstexprStringView< A >, ConstexprStringView< Y > ...)`

#### Template
||||
|---:|:---|:---|
|char|A||
|char...|X||
|char...|Y||
#### Parameters
||||
|---:|:---|:---|
|ConstexprStringView< X... >|None||
|ConstexprStringView< A >|None||
|ConstexprStringView< Y >|...||
------
### `template<char... C> auto typeek(ConstexprStringView< C... >)`

#### Template
||||
|---:|:---|:---|
|char...|C||
#### Parameters
||||
|---:|:---|:---|
|ConstexprStringView< C... >|None||
------
### `template<SizeType index> constexpr NoType TupleChooseN()`

#### Template
||||
|---:|:---|:---|
|SizeType|index||
------
### `template<SizeType index, class T, class... Ts, typename typeTraits::enableIf<> constexpr NoType TupleChooseN(T && t, Ts &&... ts)`

#### Template
||||
|---:|:---|:---|
|SizeType|index||
|class T|None||
|class...|Ts||
|typename typeTraits::enableIf<|None||
#### Parameters
||||
|---:|:---|:---|
|T &&|t||
|Ts &&...|ts||
------
### `template<SizeType index, class T, class... Ts, typename typeTraits::enableIf< index==0 >::type *> constexpr decltype(auto) TupleChooseN(T && t, Ts &&... ts)`

#### Template
||||
|---:|:---|:---|
|SizeType|index||
|class T|None||
|class...|Ts||
|typename typeTraits::enableIf< index==0 >::type *|None||
#### Parameters
||||
|---:|:---|:---|
|T &&|t||
|Ts &&...|ts||
------
### `void contextSwitch(void ** stack1, void * stack2)`

#### Parameters
||||
|---:|:---|:---|
|void **|stack1||
|void *|stack2||
------
### `void * contextTask()`

------
### `template<class T> typedef TupleTypeOf`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<SizeType L> typedef TupleRange`

#### Template
||||
|---:|:---|:---|
|SizeType|L||
