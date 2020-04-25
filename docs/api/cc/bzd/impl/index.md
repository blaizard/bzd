# [`bzd`](../../index.md)::[`impl`](../index.md)

## ` impl`


|Class||
|:---|:---|
|[`BTree`](btree/index.md)||
|[`Buffer`](buffer/index.md)||
|[`ComputeMethodIdentical`](computemethodidentical/index.md)||
|[`Expected`](expected/index.md)||
|[`Expected< void, E >`](expected_void_e_/index.md)||
|[`IChannel`](ichannel/index.md)||
|[`IOChannel`](iochannel/index.md)||
|[`IOChannelCommon`](iochannelcommon/index.md)||
|[`Log`](log/index.md)||
|[`Map`](map/index.md)|Flat map implementation.|
|[`OChannel`](ochannel/index.md)||
|[`Optional`](optional/index.md)||
|[`Pool`](pool/index.md)||
|[`Queue`](queue/index.md)||
|[`Registry`](registry/index.md)||
|[`SingleLinkedPool`](singlelinkedpool/index.md)||
|[`String`](string/index.md)||
|[`StringChannel`](stringchannel/index.md)||
|[`StringView`](stringview/index.md)||
|[`TupleElem`](tupleelem/index.md)||
|[`TupleImpl`](tupleimpl/index.md)||
|[`TupleImpl< TupleSizes< N... >, T... >`](tupleimpl_tuplesizes_n_t_/index.md)||
|[`Unexpected`](unexpected/index.md)||
|[`Variant`](variant/index.md)||
|[`Vector`](vector/index.md)||

|Struct||
|:---|:---|
|[`AlignedStorage`](alignedstorage/index.md)||
|[`NoType`](notype/index.md)||
|[`SignalInternals`](signalinternals/index.md)||
|[`SignalInternals< 1 >`](signalinternals_1_/index.md)||
|[`SignalInternals< 2 >`](signalinternals_2_/index.md)||
|[`SignalInternals< 3 >`](signalinternals_3_/index.md)||
|[`SignalInternals< 4 >`](signalinternals_4_/index.md)||
|[`SignalInternals< 5 >`](signalinternals_5_/index.md)||
|[`SignalInternals< 6 >`](signalinternals_6_/index.md)||
|[`SignalInternals< 7 >`](signalinternals_7_/index.md)||
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
### `template<class T> class Buffer`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `class ComputeMethodIdentical`

------
### `template<class T, class E> class Expected`

#### Template
||||
|---:|:---|:---|
|class T|None||
|class E|None||
------
### `template<class E> class Expected< void, E >`

#### Template
||||
|---:|:---|:---|
|class E|None||
------
### `template<class T> class IChannel`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> class IOChannel`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `class IOChannelCommon`

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
### `template<class T> class OChannel`

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
### `template<class T, class CapacityType> class Pool`

#### Template
||||
|---:|:---|:---|
|class T|None||
|class CapacityType|None||
------
### `template<class T, class CapacityType> class Queue`

#### Template
||||
|---:|:---|:---|
|class T|None||
|class CapacityType|None||
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
|class T|None||
|class CapacityType|None||
------
### `template<class T, class Impl> class String`

#### Template
||||
|---:|:---|:---|
|class T|None||
|class Impl|None||
------
### `class StringChannel`

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
|class StorageType|None||
|class...|Ts||
------
### `template<class T, class Impl> class Vector`

#### Template
||||
|---:|:---|:---|
|class T|None||
|class Impl|None||
------
### `template<SizeType Len, SizeType Align> struct AlignedStorage`

#### Template
||||
|---:|:---|:---|
|SizeType|Len||
|SizeType|Align||
------
### `struct NoType`

------
### `template<bzd::SizeType SizeBytes> struct SignalInternals`

#### Template
||||
|---:|:---|:---|
|bzd::SizeType|SizeBytes||
------
### `struct SignalInternals< 1 >`

------
### `struct SignalInternals< 2 >`

------
### `struct SignalInternals< 3 >`

------
### `struct SignalInternals< 4 >`

------
### `struct SignalInternals< 5 >`

------
### `struct SignalInternals< 6 >`

------
### `struct SignalInternals< 7 >`

------
### `template<class T, class CapacityType> struct SingleLinkedPoolElement`

#### Template
||||
|---:|:---|:---|
|class T|None||
|class CapacityType|None||
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
### `template<SizeType Index> constexpr NoType TupleChooseN()`

#### Template
||||
|---:|:---|:---|
|SizeType|Index||
------
### `template<SizeType Index, class T, class... Ts, typeTraits::EnableIf<(Index > sizeof...(Ts))> *> constexpr NoType TupleChooseN(T && t, Ts &&... ts)`

#### Template
||||
|---:|:---|:---|
|SizeType|Index||
|class T|None||
|class...|Ts||
|typeTraits::EnableIf<(Index > sizeof...(Ts))> *|None||
#### Parameters
||||
|---:|:---|:---|
|T &&|t||
|Ts &&...|ts||
------
### `template<SizeType Index, class T, class... Ts, typeTraits::EnableIf< Index==0 > *> constexpr decltype(auto) TupleChooseN(T && t, Ts &&... ts)`

#### Template
||||
|---:|:---|:---|
|SizeType|Index||
|class T|None||
|class...|Ts||
|typeTraits::EnableIf< Index==0 > *|None||
#### Parameters
||||
|---:|:---|:---|
|T &&|t||
|Ts &&...|ts||
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
