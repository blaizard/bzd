#pragma once

#include "bzd/types.h"

// Implementation inspired by https://github.com/irrequietus/typestring

namespace bzd
{
	namespace impl
	{
		template <char... C>
		class ConstexprStringView
		{
		public:
			using DataType = const char;
			using ConstIterator = DataType*;

			constexpr ConstexprStringView() = default;
			constexpr ConstexprStringView(const char* str) : str_(str) {}

			static constexpr const char* data() noexcept { return data_; }
			constexpr const char* str() const noexcept { return str_; }

			static constexpr SizeType size() noexcept { return sizeof...(C); }

		private:
			static constexpr char const data_[sizeof...(C) + 1] = {C..., '\0'};
			const char* str_;
		};

		template<int N, int M>
		constexpr char tygrab(char const(&c)[M]) noexcept
		{ return c[N < M ? N : M-1]; }

		template<char... X>
		auto typoke(ConstexprStringView<X...>) -> ConstexprStringView<X...>;

		template<char... X, char... Y>
		auto typoke(ConstexprStringView<X...>, ConstexprStringView<'\0'>, ConstexprStringView<Y>...) -> ConstexprStringView<X...>;

		template<char A, char... X, char... Y>
		auto typoke(ConstexprStringView<X...>, ConstexprStringView<A>, ConstexprStringView<Y>...) -> decltype(typoke(ConstexprStringView<X...,A>(), ConstexprStringView<Y>()...));

		template<char... C>
		auto typeek(ConstexprStringView<C...>) -> decltype(typoke(ConstexprStringView<C>()...));
	}
}

// 2^0 = 1
#define CONSTEXPR_STRING_VIEW_1(n,x) bzd::impl::tygrab<0x##n##0>(x)

// 2^1 = 2
#define CONSTEXPR_STRING_VIEW_2(n,x) bzd::impl::tygrab<0x##n##0>(x), bzd::impl::tygrab<0x##n##1>(x) 

// 2^2 = 2
#define CONSTEXPR_STRING_VIEW_4(n,x) \
        bzd::impl::tygrab<0x##n##0>(x), bzd::impl::tygrab<0x##n##1>(x) \
      , bzd::impl::tygrab<0x##n##2>(x), bzd::impl::tygrab<0x##n##3>(x)    

// 2^3 = 8
#define CONSTEXPR_STRING_VIEW_8(n,x) \
        bzd::impl::tygrab<0x##n##0>(x), bzd::impl::tygrab<0x##n##1>(x) \
      , bzd::impl::tygrab<0x##n##2>(x), bzd::impl::tygrab<0x##n##3>(x) \
      , bzd::impl::tygrab<0x##n##4>(x), bzd::impl::tygrab<0x##n##5>(x) \
      , bzd::impl::tygrab<0x##n##6>(x), bzd::impl::tygrab<0x##n##7>(x) 

// 2^4 = 16
#define CONSTEXPR_STRING_VIEW_16(n,x) \
        bzd::impl::tygrab<0x##n##0>(x), bzd::impl::tygrab<0x##n##1>(x) \
      , bzd::impl::tygrab<0x##n##2>(x), bzd::impl::tygrab<0x##n##3>(x) \
      , bzd::impl::tygrab<0x##n##4>(x), bzd::impl::tygrab<0x##n##5>(x) \
      , bzd::impl::tygrab<0x##n##6>(x), bzd::impl::tygrab<0x##n##7>(x) \
      , bzd::impl::tygrab<0x##n##8>(x), bzd::impl::tygrab<0x##n##9>(x) \
      , bzd::impl::tygrab<0x##n##A>(x), bzd::impl::tygrab<0x##n##B>(x) \
      , bzd::impl::tygrab<0x##n##C>(x), bzd::impl::tygrab<0x##n##D>(x) \
      , bzd::impl::tygrab<0x##n##E>(x), bzd::impl::tygrab<0x##n##F>(x)

// 2^5 = 32
#define CONSTEXPR_STRING_VIEW_32(n,x) \
        CONSTEXPR_STRING_VIEW_16(n##0,x),CONSTEXPR_STRING_VIEW_16(n##1,x)
      
// 2^6 = 64
#define CONSTEXPR_STRING_VIEW_64(n,x) \
        CONSTEXPR_STRING_VIEW_16(n##0,x), CONSTEXPR_STRING_VIEW_16(n##1,x), CONSTEXPR_STRING_VIEW_16(n##2,x) \
      , CONSTEXPR_STRING_VIEW_16(n##3,x)

// 2^7 = 128
#define CONSTEXPR_STRING_VIEW_128(n,x) \
        CONSTEXPR_STRING_VIEW_16(n##0,x), CONSTEXPR_STRING_VIEW_16(n##1,x), CONSTEXPR_STRING_VIEW_16(n##2,x) \
      , CONSTEXPR_STRING_VIEW_16(n##3,x), CONSTEXPR_STRING_VIEW_16(n##4,x), CONSTEXPR_STRING_VIEW_16(n##5,x) \
      , CONSTEXPR_STRING_VIEW_16(n##6,x), CONSTEXPR_STRING_VIEW_16(n##7,x)

// 2^8 = 256
#define CONSTEXPR_STRING_VIEW_256(n,x) \
        CONSTEXPR_STRING_VIEW_16(n##0,x), CONSTEXPR_STRING_VIEW_16(n##1,x), CONSTEXPR_STRING_VIEW_16(n##2,x) \
      , CONSTEXPR_STRING_VIEW_16(n##3,x), CONSTEXPR_STRING_VIEW_16(n##4,x), CONSTEXPR_STRING_VIEW_16(n##5,x) \
      , CONSTEXPR_STRING_VIEW_16(n##6,x), CONSTEXPR_STRING_VIEW_16(n##7,x), CONSTEXPR_STRING_VIEW_16(n##8,x) \
      , CONSTEXPR_STRING_VIEW_16(n##9,x), CONSTEXPR_STRING_VIEW_16(n##A,x), CONSTEXPR_STRING_VIEW_16(n##B,x) \
      , CONSTEXPR_STRING_VIEW_16(n##C,x), CONSTEXPR_STRING_VIEW_16(n##D,x), CONSTEXPR_STRING_VIEW_16(n##E,x) \
      , CONSTEXPR_STRING_VIEW_16(n##F,x)

#define CONSTEXPR_STRING_VIEW(x) decltype(bzd::impl::typeek(bzd::impl::ConstexprStringView<CONSTEXPR_STRING_VIEW_256(,x)>()))(x)
