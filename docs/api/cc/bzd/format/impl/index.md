# [`bzd`](../../../index.md)::[`format`](../../index.md)::[`impl`](../index.md)

## ` impl`


|Namespace||
|:---|:---|
|[`@27`](_27/index.md)||

|Class||
|:---|:---|
|[`CheckContext`](checkcontext/index.md)||
|[`ConstexprVector`](constexprvector/index.md)||
|[`Context`](context/index.md)||
|[`Formatter`](formatter/index.md)||
|[`FormatterSpecialized`](formatterspecialized/index.md)||
|[`HasFormatter`](hasformatter/index.md)||
|[`HasFormatterWithMetadata`](hasformatterwithmetadata/index.md)||
|[`PrintContext`](printcontext/index.md)||

|Struct||
|:---|:---|
|[`Metadata`](metadata/index.md)||

|Function||
|:---|:---|
|[`contextBuild(const bzd::StringView & format, const T & tuple)`](./index.md)||
|[`contextCheck(const Ctx & context, const T & tuple)`](./index.md)|Check the format context.|
|[`fixedPoint(bzd::OChannel & stream, const T & n, const SizeType maxPrecision)`](./index.md)||
|[`integer(interface::String & str, const T & n, const char *const digits)`](./index.md)||
|[`parse(Ctx & context, bzd::StringView format, const T & args)`](./index.md)||
|[`parseIndex(Ctx & context, bzd::StringView & format, const bzd::SizeType autoIndex)`](./index.md)||
|[`parseMetadata(Ctx & context, bzd::StringView & format, const bzd::SizeType current)`](./index.md)|Parse a metadata conversion string.|
|[`parseSign(Ctx & context, bzd::StringView & format, Metadata & metadata)`](./index.md)||
|[`parseStaticString(Ctx & context, bzd::StringView & format)`](./index.md)||
|[`parseUnsignedInteger(bzd::StringView & format, bzd::SizeType & integer)`](./index.md)||
|[`print(bzd::OChannel & stream, const bzd::StringView & format, const bzd::interface::Vector< const bzd::format::impl::Formatter * > & args)`](./index.md)||
|[`printFixedPoint(bzd::OChannel & stream, const T & value, const Metadata & metadata)`](./index.md)||
|[`printInteger(bzd::OChannel & stream, const T & value, const Metadata & metadata)`](./index.md)||
|[`toString(bzd::OChannel & stream, const T & value, const Metadata & metadata)`](./index.md)||
|[`toString(bzd::OChannel & stream, const bzd::StringView stringView, const Metadata & metadata)`](./index.md)||
|[`toStringRuntime(bzd::OChannel & out, const bzd::StringView & str, bzd::meta::range::Type< I... >, Args &&... args)`](./index.md)||
------
### `class CheckContext`

------
### `template<class T, SizeType N> class ConstexprVector`
Simple vector container working with conxtexpr
#### Template
||||
|---:|:---|:---|
|class T|None||
|SizeType|N||
------
### `template<class T> class Context`
Context used for the current parsing operation. Different context are used to check or print the formated string.
#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `class Formatter`
Type removal of the type-specific formatter.
------
### `template<class T> class FormatterSpecialized`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> class HasFormatter`

#### Template
||||
|---:|:---|:---|
|class T|None||
------
### `template<class T> class HasFormatterWithMetadata`

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
### `template<SizeType N, class Ctx, class T, bzd::typeTraits::EnableIf<(N > 0)> *> constexpr bool contextCheck(const Ctx & context, const T & tuple)`
Check the format context.

Check the format context with the argument type, this to ensure type safety. This function should only be used at compile time.
#### Template
||||
|---:|:---|:---|
|SizeType|N||
|class Ctx|None||
|class T|None||
|bzd::typeTraits::EnableIf<(N > 0)> *|None||
#### Parameters
||||
|---:|:---|:---|
|const Ctx &|context||
|const T &|tuple||
------
### `template<class T> constexpr void fixedPoint(bzd::OChannel & stream, const T & n, const SizeType maxPrecision)`

#### Template
||||
|---:|:---|:---|
|class T|None||
#### Parameters
||||
|---:|:---|:---|
|bzd::OChannel &|stream||
|const T &|n||
|const SizeType|maxPrecision||
------
### `template<SizeType Base, class T, bzd::typeTraits::EnableIf<(Base > 1 &&Base<=16), T > *> constexpr void integer(interface::String & str, const T & n, const char *const digits)`

#### Template
||||
|---:|:---|:---|
|SizeType|Base||
|class T|None||
|bzd::typeTraits::EnableIf<(Base > 1 &&Base<=16), T > *|None||
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
### `static void print(bzd::OChannel & stream, const bzd::StringView & format, const bzd::interface::Vector< const bzd::format::impl::Formatter * > & args)`

#### Parameters
||||
|---:|:---|:---|
|bzd::OChannel &|stream||
|const bzd::StringView &|format||
|const bzd::interface::Vector< const bzd::format::impl::Formatter * > &|args||
------
### `template<class T> void printFixedPoint(bzd::OChannel & stream, const T & value, const Metadata & metadata)`

#### Template
||||
|---:|:---|:---|
|class T|None||
#### Parameters
||||
|---:|:---|:---|
|bzd::OChannel &|stream||
|const T &|value||
|const Metadata &|metadata||
------
### `template<class T> void printInteger(bzd::OChannel & stream, const T & value, const Metadata & metadata)`

#### Template
||||
|---:|:---|:---|
|class T|None||
#### Parameters
||||
|---:|:---|:---|
|bzd::OChannel &|stream||
|const T &|value||
|const Metadata &|metadata||
------
### `template<class T, bzd::typeTraits::EnableIf< bzd::typeTraits::isIntegral< T >, void > *> void toString(bzd::OChannel & stream, const T & value, const Metadata & metadata)`

#### Template
||||
|---:|:---|:---|
|class T|None||
|bzd::typeTraits::EnableIf< bzd::typeTraits::isIntegral< T >, void > *|None||
#### Parameters
||||
|---:|:---|:---|
|bzd::OChannel &|stream||
|const T &|value||
|const Metadata &|metadata||
------
### `static void toString(bzd::OChannel & stream, const bzd::StringView stringView, const Metadata & metadata)`

#### Parameters
||||
|---:|:---|:---|
|bzd::OChannel &|stream||
|const bzd::StringView|stringView||
|const Metadata &|metadata||
------
### `template<SizeType... I, class... Args> constexpr void toStringRuntime(bzd::OChannel & out, const bzd::StringView & str, bzd::meta::range::Type< I... >, Args &&... args)`

#### Template
||||
|---:|:---|:---|
|SizeType...|I||
|class...|Args||
#### Parameters
||||
|---:|:---|:---|
|bzd::OChannel &|out||
|const bzd::StringView &|str||
|bzd::meta::range::Type< I... >|None||
|Args &&...|args||
