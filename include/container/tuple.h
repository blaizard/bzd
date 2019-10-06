#pragma once

#include "include/types.h"
#include "include/utility.h"
#include "include/type_traits/utils.h"

#include <utility>

namespace bzd
{
	namespace impl
	{
		template <class T>
		struct TupleId { using type = T; };

		template <class T>
		using TupleTypeOf = typename T::type;

		template <SizeType... N>
		struct TupleSizes : TupleId <TupleSizes<N...>>{};

		// choose N-th element in list <T...>
		template <SizeType N, class... T>
		struct TupleChooseImpl;

		template <SizeType N, class H, class... T>
		struct TupleChooseImpl<N, H, T...> : TupleChooseImpl<N-1, T...>{};

		template <class H, class... T>
		struct TupleChooseImpl<0, H, T...> : TupleId<H>{};

		template <SizeType N, class... T>
		using TupleChoose = TupleTypeOf<TupleChooseImpl<N, T...>>;

		// given L>=0, generate sequence <0, ..., L-1>

		template <SizeType L, SizeType I = 0, class S = TupleSizes<>>
		struct TupleRangeImpl;

		template <SizeType L, SizeType I, SizeType... N>
		struct TupleRangeImpl<L, I, TupleSizes<N...>> : TupleRangeImpl<L, I+1, TupleSizes<N..., I>>{};

		template <SizeType L, SizeType... N>
		struct TupleRangeImpl<L, L, TupleSizes<N...>> : TupleSizes<N...>{};

		template <SizeType L>
		using TupleRange = TupleTypeOf<TupleRangeImpl<L>>;

		// TupleChooseN

		struct NoType {};

		template<SizeType index>
		constexpr NoType TupleChooseN() { return NoType{}; }

		template<SizeType index, class T, class... Ts, typename typeTraits::enableIf<(index > sizeof...(Ts))>::type* = nullptr>
		constexpr NoType TupleChooseN(T&& t, Ts&&... ts)
		{
			return NoType{};
		}

		template<SizeType index, class T, class... Ts, typename typeTraits::enableIf<index == 0>::type* = nullptr>
		constexpr decltype(auto) TupleChooseN(T&& t, Ts&&... ts)
		{
			return bzd::forward<T>(t); 
		}

		template<SizeType index, class T, class... Ts, typename typeTraits::enableIf<(index > 0 && index <= sizeof...(Ts))>::type* = nullptr>
		constexpr decltype(auto) TupleChooseN(T&& t, Ts&&... ts)
		{
			return TupleChooseN<index-1>(bzd::forward<Ts>(ts)...);
		}

		// single tuple element
		template <SizeType N, class T>
		class TupleElem
		{
		private:
			T elem_;

		public:
			constexpr TupleElem() noexcept = default;
			constexpr TupleElem(const T& value) noexcept : elem_(value) {}
			constexpr TupleElem(const NoType&) noexcept : elem_() {}

			constexpr T& get() noexcept { return elem_; }
			constexpr const T& get() const noexcept { return elem_; }
		};

		// tuple implementation
		template <class N, class... T>
		class TupleImpl;

		template <SizeType... N, class... T>
		class TupleImpl<TupleSizes<N...>, T...> : TupleElem<N, T>...
		{
		private:
			template <SizeType M> using pick = TupleChoose<M, T...>;
			template <SizeType M> using elem = TupleElem<M, pick<M>>;

		public:
			template <class... Args, typename Indices = std::make_index_sequence<sizeof...(Args)>>
			constexpr TupleImpl(Args&&... args) noexcept
					: TupleElem<N, T>(TupleChooseN<N, Args...>(bzd::forward<Args>(args)...))...
			{
			}

			template <SizeType M>
			constexpr pick<M>& get() noexcept { return elem<M>::get(); }

			template <SizeType M>
			constexpr const pick<M>& get() const noexcept { return elem<M>::get(); }
		};
	}

	template <class... T>
	struct Tuple : impl::TupleImpl<impl::TupleRange<sizeof...(T)>, T...>
	{
		static constexpr SizeType size() noexcept { return sizeof...(T); }
		using impl::TupleImpl<impl::TupleRange<sizeof...(T)>, T...>::TupleImpl;
	};
}
