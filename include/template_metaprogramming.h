#pragma once

#include "include/types.h"
#include "include/utility.h"

namespace bzd
{
	namespace tmp
	{
		// Variadic Max
	
		template <class T>
		constexpr T variadicMax(T&& a) { return bzd::forward<T>(a); }

		template <class T, class... Args>
		constexpr T variadicMax(T&& a, T&& b, Args&&... args)
		{
			return (a > b) ? variadicMax(bzd::forward<T>(a), bzd::forward<Args>(args)...) : variadicMax(bzd::forward<T>(b), bzd::forward<Args>(args)...);
		}

		// Type

		template <class T>
		struct Type { using type = T; };

		// ChooseNth
		// Choose N-th element in list <Type<T>...>

		namespace impl
		{
			template <SizeType N, class... Ts>
			struct ChooseNth;

			template <SizeType N, class H, class... Ts>
			struct ChooseNth<N, H, Ts...> : ChooseNth<N - 1, Ts...> {};

			template <class H, class... Ts>
			struct ChooseNth<0, H, Ts...> : Type<H> {};
		}

		template <SizeType N, class... Ts>
		using ChooseNth = typename impl::ChooseNth<N, Ts...>::type;

		// Contains

		namespace impl
		{
			template <class T, class U, class... Ts>
			struct Contains
			{
				static const bool value = Contains<T, U>::value || Contains<T, Ts...>::value;
			};

			template <class T, class U>
			struct Contains<T, U>
			{
				static const bool value = bzd::typeTraits::isSame<T, U>::value;
			};
		}

		template <class T, class... Ts>
		using Contains = typename impl::Contains<T, Ts...>;

		// TypeList

		template <class... Ts>
		struct TypeList
		{
			using type = TypeList;
			static constexpr const SizeType size = sizeof...(Ts);

			template <SizeType N>
			using ChooseNth = typename impl::ChooseNth<N, Ts...>::type;

			template <class T>
			using Contains = typename impl::Contains<T, Ts...>;
		};
	}
/*
// Variadic Or
template<typename... Args>
struct Or;

template<typename T, typename... Args>
struct Or<T, Args...> {
    static constexpr bool value = T::value || Or<Args...>::value;
};

template<typename T>
struct Or<T> {
    static constexpr bool value = T::value;
};*/
}
