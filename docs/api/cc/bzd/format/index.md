# [`bzd`](../../index.md)::[`format`](../index.md)

## ` format`


|Namespace||
|:---|:---|
|[`impl`](impl/index.md)||

|Function||
|:---|:---|
|[`toString(bzd::OChannel & out, const bzd::StringView & str, Args &&... args)`](./index.md)|String formating.|
|[`toString(bzd::OChannel & out, const ConstexprStringView & str, Args &&... args)`](./index.md)||
|[`toString(bzd::OChannel & stream, const T & data)`](./index.md)||
|[`toString(bzd::OChannel & stream, const T & data, const SizeType maxPrecision)`](./index.md)||
|[`toString(bzd::OChannel & stream, const bzd::StringView & data)`](./index.md)||
|[`toString(bzd::OChannel & stream, const char c)`](./index.md)||
|[`toString(bzd::interface::String & str, Args &&... args)`](./index.md)||
|[`toStringBin(bzd::OChannel & stream, const T & data)`](./index.md)||
|[`toStringHex(bzd::OChannel & stream, const T & data, const char *const digits)`](./index.md)||
|[`toStringOct(bzd::OChannel & stream, const T & data)`](./index.md)||
------
### `template<class... Args> constexpr void toString(bzd::OChannel & out, const bzd::StringView & str, Args &&... args)`
String formating.

Lightweight and compilation time checking string formating utility. The syntax is compatible with Python format with some limitations.

```c++
format_spec ::= [sign][#][.precision][type]
sign ::= "+" | "-" | " "
precision ::= integer
type ::= "b" | "d" | "f" | "o" | "x" | "X" | "f" | "p" | "%"
d Decimal integer
b Binary format
o Octal format
x Hexadecimal format (lower case)
X Hexadecimal format (upper case)
f Displays fixed point number (Default: 6)
p Pointer
% Percentage. Multiples by 100 and puts % at the end.
```

This is an after text
#### Template
||||
|---:|:---|:---|
|class...|Args||
#### Parameters
||||
|---:|:---|:---|
|bzd::OChannel &|out|Output stream where the formating string will be written to. |
|const bzd::StringView &|str|run-time or compile-time string containing the format. |
|Args &&...|args|Arguments to be passed for the format. |
------
### `template<class ConstexprStringView, class... Args> constexpr void toString(bzd::OChannel & out, const ConstexprStringView & str, Args &&... args)`

#### Template
||||
|---:|:---|:---|
|class ConstexprStringView|None||
|class...|Args||
#### Parameters
||||
|---:|:---|:---|
|bzd::OChannel &|out||
|const ConstexprStringView &|str||
|Args &&...|args||
------
### `template<class T, bzd::typeTraits::EnableIf< typeTraits::isIntegral< T >, T > *> constexpr void toString(bzd::OChannel & stream, const T & data)`

#### Template
||||
|---:|:---|:---|
|class T|None||
|bzd::typeTraits::EnableIf< typeTraits::isIntegral< T >, T > *|None||
#### Parameters
||||
|---:|:---|:---|
|bzd::OChannel &|stream||
|const T &|data||
------
### `template<class T, bzd::typeTraits::EnableIf< typeTraits::isFloatingPoint< T >, void > *> constexpr void toString(bzd::OChannel & stream, const T & data, const SizeType maxPrecision)`

#### Template
||||
|---:|:---|:---|
|class T|None||
|bzd::typeTraits::EnableIf< typeTraits::isFloatingPoint< T >, void > *|None||
#### Parameters
||||
|---:|:---|:---|
|bzd::OChannel &|stream||
|const T &|data||
|const SizeType|maxPrecision||
------
### `void toString(bzd::OChannel & stream, const bzd::StringView & data)`

#### Parameters
||||
|---:|:---|:---|
|bzd::OChannel &|stream||
|const bzd::StringView &|data||
------
### `void toString(bzd::OChannel & stream, const char c)`

#### Parameters
||||
|---:|:---|:---|
|bzd::OChannel &|stream||
|const char|c||
------
### `template<class... Args> constexpr void toString(bzd::interface::String & str, Args &&... args)`

#### Template
||||
|---:|:---|:---|
|class...|Args||
#### Parameters
||||
|---:|:---|:---|
|bzd::interface::String &|str||
|Args &&...|args||
------
### `template<class T, bzd::typeTraits::EnableIf< typeTraits::isIntegral< T >, T > *> constexpr void toStringBin(bzd::OChannel & stream, const T & data)`

#### Template
||||
|---:|:---|:---|
|class T|None||
|bzd::typeTraits::EnableIf< typeTraits::isIntegral< T >, T > *|None||
#### Parameters
||||
|---:|:---|:---|
|bzd::OChannel &|stream||
|const T &|data||
------
### `template<class T, bzd::typeTraits::EnableIf< typeTraits::isIntegral< T >, void > *> constexpr void toStringHex(bzd::OChannel & stream, const T & data, const char *const digits)`

#### Template
||||
|---:|:---|:---|
|class T|None||
|bzd::typeTraits::EnableIf< typeTraits::isIntegral< T >, void > *|None||
#### Parameters
||||
|---:|:---|:---|
|bzd::OChannel &|stream||
|const T &|data||
|const char *const|digits||
------
### `template<class T, bzd::typeTraits::EnableIf< typeTraits::isIntegral< T >, T > *> constexpr void toStringOct(bzd::OChannel & stream, const T & data)`

#### Template
||||
|---:|:---|:---|
|class T|None||
|bzd::typeTraits::EnableIf< typeTraits::isIntegral< T >, T > *|None||
#### Parameters
||||
|---:|:---|:---|
|bzd::OChannel &|stream||
|const T &|data||
