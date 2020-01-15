#pragma once

#include "bzd/types.h"

// Implementation inspired by https://github.com/irrequietus/typestring

namespace bzd {
namespace impl {

template <char... C>
class StringConstexpr
{
public:
	using DataType = const char;
	using ConstIterator = DataType*;

	constexpr StringConstexpr() = default;
	constexpr StringConstexpr(const char* str) : str_(str) {}

	static constexpr const char* data() noexcept { return data_; }
	constexpr const char* str() const noexcept { return str_; }

	static constexpr SizeType size() noexcept { return sizeof...(C); }

private:
	static constexpr char const data_[sizeof...(C) + 1] = {C..., '\0'};
	const char* str_;
};

template <int N, int M>
constexpr char tygrab(char const (&c)[M]) noexcept
{
	return c[N < M ? N : M - 1];
}

template <char... X>
auto typoke(StringConstexpr<X...>) -> StringConstexpr<X...>;

template <char... X, char... Y>
auto typoke(StringConstexpr<X...>, StringConstexpr<'\0'>, StringConstexpr<Y>...) -> StringConstexpr<X...>;

template <char A, char... X, char... Y>
auto typoke(StringConstexpr<X...>, StringConstexpr<A>, StringConstexpr<Y>...)
	-> decltype(typoke(StringConstexpr<X..., A>(), StringConstexpr<Y>()...));

template <char... C>
auto typeek(StringConstexpr<C...>) -> decltype(typoke(StringConstexpr<C>()...));
} // namespace impl

template <char... C>
using StringConstexpr = impl::StringConstexpr<C...>;

} // namespace bzd

// 2^0 = 1
#define STRING_CONSTEXPR_1_(n, x) bzd::impl::tygrab<0x##n##0>(x)

// 2^1 = 2
#define STRING_CONSTEXPR_2_(n, x) bzd::impl::tygrab<0x##n##0>(x), bzd::impl::tygrab<0x##n##1>(x)

// 2^2 = 2
#define STRING_CONSTEXPR_4_(n, x) \
	bzd::impl::tygrab<0x##n##0>(x), bzd::impl::tygrab<0x##n##1>(x), bzd::impl::tygrab<0x##n##2>(x), bzd::impl::tygrab<0x##n##3>(x)

// 2^3 = 8
#define STRING_CONSTEXPR_8_(n, x)                                                                                                   \
	bzd::impl::tygrab<0x##n##0>(x), bzd::impl::tygrab<0x##n##1>(x), bzd::impl::tygrab<0x##n##2>(x), bzd::impl::tygrab<0x##n##3>(x), \
		bzd::impl::tygrab<0x##n##4>(x), bzd::impl::tygrab<0x##n##5>(x), bzd::impl::tygrab<0x##n##6>(x), bzd::impl::tygrab<0x##n##7>(x)

// 2^4 = 16
#define STRING_CONSTEXPR_16_(n, x)                                                                                                      \
	bzd::impl::tygrab<0x##n##0>(x), bzd::impl::tygrab<0x##n##1>(x), bzd::impl::tygrab<0x##n##2>(x), bzd::impl::tygrab<0x##n##3>(x),     \
		bzd::impl::tygrab<0x##n##4>(x), bzd::impl::tygrab<0x##n##5>(x), bzd::impl::tygrab<0x##n##6>(x), bzd::impl::tygrab<0x##n##7>(x), \
		bzd::impl::tygrab<0x##n##8>(x), bzd::impl::tygrab<0x##n##9>(x), bzd::impl::tygrab<0x##n##A>(x), bzd::impl::tygrab<0x##n##B>(x), \
		bzd::impl::tygrab<0x##n##C>(x), bzd::impl::tygrab<0x##n##D>(x), bzd::impl::tygrab<0x##n##E>(x), bzd::impl::tygrab<0x##n##F>(x)

// 2^5 = 32
#define STRING_CONSTEXPR_32_(n, x) STRING_CONSTEXPR_16_(n##0, x), STRING_CONSTEXPR_16_(n##1, x)

// 2^6 = 64
#define STRING_CONSTEXPR_64_(n, x) \
	STRING_CONSTEXPR_16_(n##0, x), STRING_CONSTEXPR_16_(n##1, x), STRING_CONSTEXPR_16_(n##2, x), STRING_CONSTEXPR_16_(n##3, x)

// 2^7 = 128
#define STRING_CONSTEXPR_128_(n, x)                                                                                             \
	STRING_CONSTEXPR_16_(n##0, x), STRING_CONSTEXPR_16_(n##1, x), STRING_CONSTEXPR_16_(n##2, x), STRING_CONSTEXPR_16_(n##3, x), \
		STRING_CONSTEXPR_16_(n##4, x), STRING_CONSTEXPR_16_(n##5, x), STRING_CONSTEXPR_16_(n##6, x), STRING_CONSTEXPR_16_(n##7, x)

// 2^8 = 256
#define STRING_CONSTEXPR_256_(n, x)                                                                                                 \
	STRING_CONSTEXPR_16_(n##0, x), STRING_CONSTEXPR_16_(n##1, x), STRING_CONSTEXPR_16_(n##2, x), STRING_CONSTEXPR_16_(n##3, x),     \
		STRING_CONSTEXPR_16_(n##4, x), STRING_CONSTEXPR_16_(n##5, x), STRING_CONSTEXPR_16_(n##6, x), STRING_CONSTEXPR_16_(n##7, x), \
		STRING_CONSTEXPR_16_(n##8, x), STRING_CONSTEXPR_16_(n##9, x), STRING_CONSTEXPR_16_(n##A, x), STRING_CONSTEXPR_16_(n##B, x), \
		STRING_CONSTEXPR_16_(n##C, x), STRING_CONSTEXPR_16_(n##D, x), STRING_CONSTEXPR_16_(n##E, x), STRING_CONSTEXPR_16_(n##F, x)

#define CSTR(x) decltype(bzd::impl::typeek(bzd::StringConstexpr<STRING_CONSTEXPR_256_(, x)>()))(x)
