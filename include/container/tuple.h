#pragma once

#include "include/types.h"

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

		// single tuple element
		template <SizeType N, class T>
		class TupleElem
		{
		private:
			T elem;
		public:
			T& get() { return elem; }
			const T& get() const { return elem; }
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
			template <SizeType M>
			pick<M>& get() { return elem<M>::get(); }

			template <SizeType M>
			const pick<M>& get() const { return elem<M>::get(); }
		};
	}

	template <class... T>
	struct Tuple : impl::TupleImpl<impl::TupleRange<sizeof...(T)>, T...>
	{
		static constexpr std::size_t size() { return sizeof...(T); }
	};
}
