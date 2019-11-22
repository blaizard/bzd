# [`bzd`](../../index.md)::[`impl`](../index.md)

## ` impl`

### Class
||||
|---:|:---|:---|
|class|[`IOStream`](./iostream/index.md)||
|class|[`Variant`](./variant/index.md)||
|class|[`ConstexprStringView`](./constexprstringview/index.md)||
|class|[`Vector`](./vector/index.md)||
|class|[`Expected`](./expected/index.md)||
|class|[`IStream`](./istream/index.md)||
|class|[`Expected< void, E >`](./expected_void_e_/index.md)||
|class|[`TupleImpl`](./tupleimpl/index.md)||
|class|[`StringView`](./stringview/index.md)||
|class|[`Unexpected`](./unexpected/index.md)||
|class|[`String`](./string/index.md)||
|class|[`TupleElem`](./tupleelem/index.md)||
|class|[`OStream`](./ostream/index.md)||
|class|[`TupleImpl< TupleSizes< N... >, T... >`](./tupleimpl_tuplesizes_n_t_/index.md)||
|class|[`StringStream`](./stringstream/index.md)||
### Struct
||||
|---:|:---|:---|
|struct|[`TupleRangeImpl`](./tuplerangeimpl/index.md)||
|struct|[`NoType`](./notype/index.md)||
|struct|[`TupleSizes`](./tuplesizes/index.md)||
|struct|[`TupleRangeImpl< L, L, TupleSizes< N... > >`](./tuplerangeimpl_l_l_tuplesizes_n_/index.md)||
|struct|[`TupleRangeImpl< L, I, TupleSizes< N... > >`](./tuplerangeimpl_l_i_tuplesizes_n_/index.md)||
### Function
||||
|---:|:---|:---|
|constexpr char|[`tygrab(char const(&) c)`](.)||
|auto|[`typoke(ConstexprStringView< X... >)`](.)||
|auto|[`typoke(ConstexprStringView< X... >, ConstexprStringView<'\0'>, ConstexprStringView< Y > ...)`](.)||
|auto|[`typoke(ConstexprStringView< X... >, ConstexprStringView< A >, ConstexprStringView< Y > ...)`](.)||
|auto|[`typeek(ConstexprStringView< C... >)`](.)||
|constexpr NoType|[`TupleChooseN()`](.)||
|constexpr NoType|[`TupleChooseN(T && t, Ts &&... ts)`](.)||
|constexpr decltype(auto)|[`TupleChooseN(T && t, Ts &&... ts)`](.)||
|void|[`contextSwitch(void ** stack1, void * stack2)`](.)||
|void *|[`contextTask()`](.)||
### Typedef
||||
|---:|:---|:---|
|typedef|[`TupleTypeOf`](.)||
|typedef|[`TupleRange`](.)|alias of [`bzd::impl::TupleRangeImpl`](tuplerangeimpl/index.md)|
------
### `template<class T> class IOStream`

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
### `template<class T, class Impl> class StringView`

#### Template
||||
|---:|:---|:---|
|class T|None||
|class Impl|None||
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
### `template<SizeType N, class T> class TupleElem`

#### Template
||||
|---:|:---|:---|
|SizeType|N||
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
### `template<int N, int M> constexpr char tygrab(char const(&) c)`

#### Template
||||
|---:|:---|:---|
|int|N||
|int|M||
#### Parameters
||||
|---:|:---|:---|
|char const(&)|c||
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
