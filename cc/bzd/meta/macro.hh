#pragma once

/// Returns 1 if the argument is empty, 0 ortherwise.
#define BZD_IS_EMPTY(...) BZD_IS_EMPTY_X(1, 0, __VA_ARGS__)

/// Returns tvalue if the argument is empty, fvalue otherwise.
#define BZD_IS_EMPTY_X(tvalue, fvalue, ...) BZD_INTERNAL_IS_EMPTY_X0(tvalue, fvalue, __VA_ARGS__)
#define BZD_INTERNAL_IS_EMPTY_X0(tvalue, fvalue, first, ...) BZD_INTERNAL_IS_EMPTY_X1(tvalue, fvalue, BZD_INTERNAL_IS_EMPTY_PROBE first())
#define BZD_INTERNAL_IS_EMPTY_X1(tvalue, fvalue, ...) BZD_INTERNAL_IS_EMPTY_X2(__VA_ARGS__, tvalue, fvalue)
#define BZD_INTERNAL_IS_EMPTY_X2(_0, _1, _2, ...) _2
#define BZD_INTERNAL_IS_EMPTY_PROBE(...) ,

/// Add quotes to the argument.
#define BZD_QUOTE(x) BZD_INTERNAL_QUOTE(x)
// NOLINTNEXTLINE(bugprone-reserved-identifier)
#define BZD_INTERNAL_QUOTE(x) #x

/// Count the number of arguments passed to the macro.
#define BZD_NARGS(...) BZD_INTERNAL_NARGS(__VA_ARGS__, BZD_INTERNAL_NARGS_SEQ_N(__VA_ARGS__))
#define BZD_INTERNAL_NARGS(...) BZD_INTERNAL_NARGS_N(__VA_ARGS__)
#define BZD_INTERNAL_NARGS_N(_1,  \
							 _2,  \
							 _3,  \
							 _4,  \
							 _5,  \
							 _6,  \
							 _7,  \
							 _8,  \
							 _9,  \
							 _10, \
							 _11, \
							 _12, \
							 _13, \
							 _14, \
							 _15, \
							 _16, \
							 _17, \
							 _18, \
							 _19, \
							 _20, \
							 _21, \
							 _22, \
							 _23, \
							 _24, \
							 _25, \
							 _26, \
							 _27, \
							 _28, \
							 _29, \
							 _30, \
							 _31, \
							 _32, \
							 _33, \
							 _34, \
							 _35, \
							 _36, \
							 _37, \
							 _38, \
							 _39, \
							 _40, \
							 _41, \
							 _42, \
							 _43, \
							 _44, \
							 _45, \
							 _46, \
							 _47, \
							 _48, \
							 _49, \
							 _50, \
							 _51, \
							 _52, \
							 _53, \
							 _54, \
							 _55, \
							 _56, \
							 _57, \
							 _58, \
							 _59, \
							 _60, \
							 _61, \
							 _62, \
							 _63, \
							 N,   \
							 ...) \
	N
#define BZD_INTERNAL_NARGS_SEQ_N(x, ...)                                                                                                \
	63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, \
		30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2,                     \
		BZD_IS_EMPTY_X(0, 1, x), 0

/// Call a specific macro based on the number of arguments.
#define BZD_GET_MACRO(macro, ...) BZD_INTERNAL_GET_MACRO0(macro, BZD_NARGS(__VA_ARGS__))
#define BZD_INTERNAL_GET_MACRO0(macro, nbArgs) BZD_INTERNAL_GET_MACRO1(macro, nbArgs)
#define BZD_INTERNAL_GET_MACRO1(macro, nbArgs) macro##nbArgs

/// Paste macro names together
#define BZD_PASTE(...) BZD_GET_MACRO(BZD_INTERNAL_PASTE, __VA_ARGS__)(__VA_ARGS__)
#define BZD_INTERNAL_PASTE2(a, b) a##b
#define BZD_INTERNAL_PASTE3(a, b, c) a##b##c
#define BZD_INTERNAL_PASTE4(a, b, c, d) a##b##c##d
#define BZD_INTERNAL_PASTE5(a, b, c, d, e) a##b##c##d##e
#define BZD_INTERNAL_PASTE6(a, b, c, d, e, f) a##b##c##d##e##f
#define BZD_INTERNAL_PASTE7(a, b, c, d, e, f, g) a##b##c##d##e##f##g
