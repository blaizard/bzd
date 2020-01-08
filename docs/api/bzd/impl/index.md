# [`bzd`](../../index.md)::[`impl`](../index.md)

## ` impl`


|Class||
|:---|:---|
|[`BTree`](btree/index.md)||
|[`Expected`](expected/index.md)||
|[`Expected< void, E >`](expected_void_e_/index.md)||
|[`IOStream`](iostream/index.md)||
|[`IStream`](istream/index.md)||
|[`Log`](log/index.md)||
|[`Map`](map/index.md)|Flat map implementation.|
|[`Optional`](optional/index.md)||
|[`OStream`](ostream/index.md)||
|[`Pool`](pool/index.md)||
|[`Queue`](queue/index.md)||
|[`Registry`](registry/index.md)||
|[`SingleLinkedPool`](singlelinkedpool/index.md)||
|[`String`](string/index.md)||
|[`StringConstexpr`](stringconstexpr/index.md)||
|[`StringStream`](stringstream/index.md)||
|[`StringView`](stringview/index.md)||
|[`TupleElem`](tupleelem/index.md)||
|[`TupleImpl`](tupleimpl/index.md)||
|[`TupleImpl< TupleSizes< N... >, T... >`](tupleimpl_tuplesizes_n_t_/index.md)||
|[`Unexpected`](unexpected/index.md)||
|[`Variant`](variant/index.md)||
|[`Vector`](vector/index.md)||

|Struct||
|:---|:---|
|[`NoType`](notype/index.md)||
|[`SingleLinkedPoolElement`](singlelinkedpoolelement/index.md)||
|[`TupleRangeImpl`](tuplerangeimpl/index.md)||
|[`TupleRangeImpl< L, I, TupleSizes< N... > >`](tuplerangeimpl_l_i_tuplesizes_n_/index.md)||
|[`TupleRangeImpl< L, L, TupleSizes< N... > >`](tuplerangeimpl_l_l_tuplesizes_n_/index.md)||
|[`TupleSizes`](tuplesizes/index.md)||

|Function||
|:---|:---|
|[`contextSwitch(void ** stack1, void * stack2)`](./index.md)||
|[`contextTask()`](./index.md)||
|[`TupleChooseN()`](./index.md)||
|[`TupleChooseN(T && t, Ts &&... ts)`](./index.md)||
|[`TupleChooseN(T && t, Ts &&... ts)`](./index.md)||
|[`tygrab(char const (&) c)`](./index.md)||
|[`typeek(StringConstexpr< C... >)`](./index.md)||
|[`typoke(StringConstexpr< X... >)`](./index.md)||
|[`typoke(StringConstexpr< X... >, StringConstexpr<'\0'>, StringConstexpr< Y > ...)`](./index.md)||
|[`typoke(StringConstexpr< X... >, StringConstexpr< A >, StringConstexpr< Y > ...)`](./index.md)||

|Typedef||
|:---|:---|
|[`TupleRange`](./index.md)|alias of [`bzd::impl::TupleRangeImpl`](tuplerangeimpl/index.md)|
|[`TupleTypeOf`](./index.md)||
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
### `template<class T, class E> class Expected`

#### Template
||||
|---:|:---|:---|
|class|T||
|class|E||
------
### `template<class E> class Expected< void, E >`

#### Template
||||
|---:|:---|:---|
|class E|None||
------
### `template<class T> class IOStream`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> class IStream`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `class Log`

------
### `template<class K, class V> class Map`
Flat map implementation.
#### Template
||||
|---:|:---|:---|
|class K|None||
|class V|None||
------
### `template<class T> class Optional`

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
### `template<class T, class CapacityType> class Pool`

#### Template
||||
|---:|:---|:---|
|class|T||
|class|CapacityType||
------
### `template<class T, class CapacityType> class Queue`

#### Template
||||
|---:|:---|:---|
|class|T||
|class|CapacityType||
------
### `template<class T> class Registry`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T, class CapacityType> class SingleLinkedPool`

#### Template
||||
|---:|:---|:---|
|class|T||
|class|CapacityType||
------
### `template<class T, class Impl> class String`

#### Template
||||
|---:|:---|:---|
|class T|None||
|class Impl|None||
------
### `template<char... C> class StringConstexpr`

#### Template
||||
|---:|:---|:---|
|char...|C||
------
### `class StringStream`

------
### `template<class T, class Impl> class StringView`

#### Template
||||
|---:|:---|:---|
|class T|None||
|class Impl|None||
------
### `template<SizeType N, class T> class TupleElem`

#### Template
||||
|---:|:---|:---|
|SizeType|N||
|class T|None||
------
### `template<class N, class... T> class TupleImpl`

#### Template
||||
|---:|:---|:---|
|class N|None||
|class...|T||
------
### `template<SizeType... N, class... T> class TupleImpl< TupleSizes< N... >, T... >`

#### Template
||||
|---:|:---|:---|
|SizeType...|N||
|class...|T||
------
### `template<class E> class Unexpected`
Internal class used to create an unexpected object type.
#### Template
||||
|---:|:---|:---|
|class E|None||
------
### `template<class StorageType, class... Ts> class Variant`

#### Template
||||
|---:|:---|:---|
|class|StorageType||
|class...|Ts||
------
### `template<class T, class Impl> class Vector`

#### Template
||||
|---:|:---|:---|
|class|T||
|class|Impl||
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
### `template<SizeType L, SizeType I, class S> struct TupleRangeImpl`

#### Template
||||
|---:|:---|:---|
|SizeType|L||
|SizeType|I||
|class S|None||
------
### `template<SizeType L, SizeType I, SizeType... N> struct TupleRangeImpl< L, I, TupleSizes< N... > >`

#### Template
||||
|---:|:---|:---|
|SizeType|L||
|SizeType|I||
|SizeType...|N||
------
### `template<SizeType L, SizeType... N> struct TupleRangeImpl< L, L, TupleSizes< N... > >`

#### Template
||||
|---:|:---|:---|
|SizeType|L||
|SizeType...|N||
------
### `template<SizeType... N> struct TupleSizes`

#### Template
||||
|---:|:---|:---|
|SizeType...|N||
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
### `template<char... C> auto typeek(StringConstexpr< C... >)`

#### Template
||||
|---:|:---|:---|
|char...|C||
#### Parameters
||||
|---:|:---|:---|
|StringConstexpr< C... >|None||
------
### `template<char... X> auto typoke(StringConstexpr< X... >)`

#### Template
||||
|---:|:---|:---|
|char...|X||
#### Parameters
||||
|---:|:---|:---|
|StringConstexpr< X... >|None||
------
### `template<char... X, char... Y> auto typoke(StringConstexpr< X... >, StringConstexpr<'\0'>, StringConstexpr< Y > ...)`

#### Template
||||
|---:|:---|:---|
|char...|X||
|char...|Y||
#### Parameters
||||
|---:|:---|:---|
|StringConstexpr< X... >|None||
|StringConstexpr<'\0'>|None||
|StringConstexpr< Y >|...||
------
### `template<char A, char... X, char... Y> auto typoke(StringConstexpr< X... >, StringConstexpr< A >, StringConstexpr< Y > ...)`

#### Template
||||
|---:|:---|:---|
|char|A||
|char...|X||
|char...|Y||
#### Parameters
||||
|---:|:---|:---|
|StringConstexpr< X... >|None||
|StringConstexpr< A >|None||
|StringConstexpr< Y >|...||
------
### `template<SizeType L> typedef TupleRange`

#### Template
||||
|---:|:---|:---|
|SizeType|L||
------
### `template<class T> typedef TupleTypeOf`

#### Template
||||
|---:|:---|:---|
|class T|None||
