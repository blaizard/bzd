# [`bzd`](../../../index.md)::[`format`](../../index.md)::[`impl`](../index.md)

## ` impl`


|Namespace||
|:---|:---|
|[`@24`](_24/index.md)||

|Class||
|:---|:---|
|[`CheckContext`](checkcontext/index.md)||
|[`ConstexprVector`](constexprvector/index.md)||
|[`Context`](context/index.md)||
|[`PrintContext`](printcontext/index.md)||

|Struct||
|:---|:---|
|[`Metadata`](metadata/index.md)||

|Function||
|:---|:---|
|[`contextBuild(const bzd::StringView & format, const T & tuple)`](./index.md)||
|[`contextCheck(const Ctx & context, const T & tuple)`](./index.md)|Check the format context.|
|[`fixedPoint(bzd::OStream & stream, const T & n, const SizeType maxPrecision)`](./index.md)||
|[`integer(interface::String & str, const T & n, const char *const digits)`](./index.md)||
|[`parse(Ctx & context, bzd::StringView format, const T & args)`](./index.md)||
|[`parseIndex(Ctx & context, bzd::StringView & format, const bzd::SizeType autoIndex)`](./index.md)||
|[`parseMetadata(Ctx & context, bzd::StringView & format, const bzd::SizeType current)`](./index.md)|Parse a metadata conversion string.|
|[`parseSign(Ctx & context, bzd::StringView & format, Metadata & metadata)`](./index.md)||
|[`parseStaticString(Ctx & context, bzd::StringView & format)`](./index.md)||
|[`parseUnsignedInteger(bzd::StringView & format, bzd::SizeType & integer)`](./index.md)||
|[`print(bzd::OStream & stream, const bzd::StringView & format, const bzd::interface::Vector< bzd::format::impl::Arg > & args)`](./index.md)||
|[`printFixedPoint(bzd::OStream & stream, const T & value, const Metadata & metadata)`](./index.md)||
|[`printInteger(bzd::OStream & stream, const T & value, const Metadata & metadata)`](./index.md)||
|[`printString(bzd::OStream & stream, const bzd::StringView stringView, const Metadata & metadata)`](./index.md)||

|Typedef||
|:---|:---|
|[`Arg`](./index.md)|alias of [`bzd::VariantConstexpr`](../../variantconstexpr/index.md)|
|[`ArgList`](./index.md)|alias of [`bzd::impl::Vector`](../../impl/vector/index.md)|
------
### `class CheckContext`

------
### `template<class T, SizeType N> class ConstexprVector`
Simple vector container working with conxtexpr
#### Template
||||
|---:|:---|:---|
|class|T||
|SizeType|N||
------
### `template<class T> class Context`
Context used for the current parsing operation. Different context are used to check or print the formated string.
#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `class PrintContext`

------
### `struct Metadata`

------
### `template<class T> constexpr Context< CheckContext > contextBuild(const bzd::StringView & format, const T & tuple)`

#### Template
||||
|---:|:---|:---|
|class T|None||
#### Parameters
||||
|---:|:---|:---|
|const bzd::StringView &|format||
|const T &|tuple||
------
### `template<SizeType N, class Ctx, class T, typename bzd::typeTraits::enableIf<(N > 0> constexpr bool contextCheck(const Ctx & context, const T & tuple)`
Check the format context.

Check the format context with the argument type, this to ensure type safety. This function should only be used at compile time.
#### Template
||||
|---:|:---|:---|
|SizeType|N||
|class Ctx|None||
|class T|None||
|typename bzd::typeTraits::enableIf<(N >|0||
#### Parameters
||||
|---:|:---|:---|
|const Ctx &|context||
|const T &|tuple||
------
### `template<class T> constexpr void fixedPoint(bzd::OStream & stream, const T & n, const SizeType maxPrecision)`

#### Template
||||
|---:|:---|:---|
|class T|None||
#### Parameters
||||
|---:|:---|:---|
|bzd::OStream &|stream||
|const T &|n||
|const SizeType|maxPrecision||
------
### `template<SizeType Base, class T, typename bzd::typeTraits::enableIf<(Base > 1 &&Base<= 16> constexpr void integer(interface::String & str, const T & n, const char *const digits)`

#### Template
||||
|---:|:---|:---|
|SizeType|Base||
|class T|None||
|typename bzd::typeTraits::enableIf<(Base > 1 &&Base<=|16||
#### Parameters
||||
|---:|:---|:---|
|interface::String &|str||
|const T &|n||
|const char *const|digits||
------
### `template<class Ctx, class T> constexpr void parse(Ctx & context, bzd::StringView format, const T & args)`

#### Template
||||
|---:|:---|:---|
|class Ctx|None||
|class T|None||
#### Parameters
||||
|---:|:---|:---|
|Ctx &|context||
|bzd::StringView|format||
|const T &|args||
------
### `template<class Ctx> constexpr bzd::SizeType parseIndex(Ctx & context, bzd::StringView & format, const bzd::SizeType autoIndex)`
Return the positional index
#### Template
||||
|---:|:---|:---|
|class Ctx|None||
#### Parameters
||||
|---:|:---|:---|
|Ctx &|context||
|bzd::StringView &|format||
|const bzd::SizeType|autoIndex||
------
### `template<class Ctx> constexpr Metadata parseMetadata(Ctx & context, bzd::StringView & format, const bzd::SizeType current)`
Parse a metadata conversion string.

Format compatible with python format (with some exceptions)

format_spec ::= [sign][#][.precision][type] sign ::= "+" | "-" | " " precision ::= integer type ::= "b" | "d" | "f" | "o" | "x" | "X" | "f" | "p" | "%" d Decimal integer b Binary format o Octal format x Hexadecimal format (lower case) X Hexadecimal format (upper case) f Displays fixed point number (Default: 6) p Pointer % Percentage. Multiples by 100 and puts % at the end.
#### Template
||||
|---:|:---|:---|
|class Ctx|None||
#### Parameters
||||
|---:|:---|:---|
|Ctx &|context||
|bzd::StringView &|format||
|const bzd::SizeType|current||
------
### `template<class Ctx> constexpr void parseSign(Ctx & context, bzd::StringView & format, Metadata & metadata)`

#### Template
||||
|---:|:---|:---|
|class Ctx|None||
#### Parameters
||||
|---:|:---|:---|
|Ctx &|context||
|bzd::StringView &|format||
|Metadata &|metadata||
------
### `template<class Ctx> constexpr bool parseStaticString(Ctx & context, bzd::StringView & format)`
Parse a static string and call addSubstring to the context information when needed.

This function returns either when the string is consumed (return false) or when a metadata has been identified.
#### Template
||||
|---:|:---|:---|
|class Ctx|None||
#### Parameters
||||
|---:|:---|:---|
|Ctx &|context||
|bzd::StringView &|format||
------
### `static constexpr bool parseUnsignedInteger(bzd::StringView & format, bzd::SizeType & integer)`
Parse an unsigned integer
#### Parameters
||||
|---:|:---|:---|
|bzd::StringView &|format||
|bzd::SizeType &|integer||
------
### `static void print(bzd::OStream & stream, const bzd::StringView & format, const bzd::interface::Vector< bzd::format::impl::Arg > & args)`

#### Parameters
||||
|---:|:---|:---|
|bzd::OStream &|stream||
|const bzd::StringView &|format||
|const bzd::interface::Vector< bzd::format::impl::Arg > &|args||
------
### `template<class T> void printFixedPoint(bzd::OStream & stream, const T & value, const Metadata & metadata)`

#### Template
||||
|---:|:---|:---|
|class T|None||
#### Parameters
||||
|---:|:---|:---|
|bzd::OStream &|stream||
|const T &|value||
|const Metadata &|metadata||
------
### `template<class T> void printInteger(bzd::OStream & stream, const T & value, const Metadata & metadata)`

#### Template
||||
|---:|:---|:---|
|class T|None||
#### Parameters
||||
|---:|:---|:---|
|bzd::OStream &|stream||
|const T &|value||
|const Metadata &|metadata||
------
### `static void printString(bzd::OStream & stream, const bzd::StringView stringView, const Metadata & metadata)`

#### Parameters
||||
|---:|:---|:---|
|bzd::OStream &|stream||
|const bzd::StringView|stringView||
|const Metadata &|metadata||
------
### `typedef Arg`

------
### `typedef ArgList`

