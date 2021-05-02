#pragma once

#include "bzd/meta/choose_nth.h"
#include "bzd/platform/types.h"
#include "bzd/type_traits/enable_if.h"
#include "bzd/type_traits/is_same.h"
#include "bzd/utility/forward.h"
#include "bzd/utility/move.h"

namespace bzd::impl {
template <class T>
using TupleTypeOf = typename T::type;

template <SizeType... N>
struct TupleSizes : bzd::meta::Type<TupleSizes<N...>>
{
};

// given L>=0, generate sequence <0, ..., L-1>

template <SizeType L, SizeType I = 0, class S = TupleSizes<>>
struct TupleRangeImpl;

template <SizeType L, SizeType I, SizeType... N>
struct TupleRangeImpl<L, I, TupleSizes<N...>> : TupleRangeImpl<L, I + 1, TupleSizes<N..., I>>
{
};

template <SizeType L, SizeType... N>
struct TupleRangeImpl<L, L, TupleSizes<N...>> : TupleSizes<N...>
{
};

template <SizeType L>
using TupleRange = TupleTypeOf<TupleRangeImpl<L>>;

// TupleChooseN

struct NoType
{
};

template <SizeType Index>
constexpr NoType TupleChooseN()
{
	return NoType{};
}

template <SizeType Index, class T, class... Ts, typeTraits::EnableIf<(Index > sizeof...(Ts))>* = nullptr>
constexpr NoType TupleChooseN(T&& t, Ts&&... ts)
{
	return NoType{};
}

template <SizeType Index, class T, class... Ts, typeTraits::EnableIf<Index == 0>* = nullptr>
constexpr decltype(auto) TupleChooseN(T&& t, Ts&&... ts)
{
	return bzd::forward<T>(t);
}

template <SizeType Index, class T, class... Ts, typeTraits::EnableIf<(Index > 0 && Index <= sizeof...(Ts))>* = nullptr>
constexpr decltype(auto) TupleChooseN(T&& t, Ts&&... ts)
{
	return TupleChooseN<Index - 1>(bzd::forward<Ts>(ts)...);
}

// single tuple element

template <SizeType N, class T>
class TupleElem
{
public:
	constexpr TupleElem() noexcept = default;
	// constexpr TupleElem(const T& value) noexcept : elem_(value) {}
	template <class Value, typeTraits::EnableIf<!typeTraits::isSame<Value, NoType>>* = nullptr>
	constexpr TupleElem(Value&& value) noexcept : elem_{bzd::forward<Value>(value)}
	{
	}
	constexpr TupleElem(const NoType&) noexcept : elem_{} {}

	constexpr T& get() noexcept { return elem_; }
	constexpr const T& get() const noexcept { return elem_; }

	constexpr void set(const T& value) noexcept { elem_ = value; }
	constexpr void set(T&& value) noexcept { elem_ = bzd::move(value); }

private:
	T elem_; //{};
};

// Tuple implementation

template <class N, class... T>
class TupleImpl;

template <SizeType... N, class... T>
class TupleImpl<TupleSizes<N...>, T...> : TupleElem<N, T>...
{
private:
	using Self = TupleImpl<TupleSizes<N...>, T...>;
	template <SizeType M>
	using Pick = bzd::meta::ChooseNth<M, T...>;
	template <SizeType M>
	using Elem = TupleElem<M, Pick<M>>;

public:
	constexpr TupleImpl() noexcept = default;

	template <class... Args>
	constexpr TupleImpl(Args&&... args) noexcept : TupleElem<N, T>{TupleChooseN<N, Args...>(bzd::forward<Args>(args)...)}...
	{
	}

	// Copy constructor/assignment.
	constexpr TupleImpl(const Self& tuple) noexcept : TupleElem<N, T>{tuple.get<N>()}... {}
	constexpr void operator=(const Self& tuple) noexcept
	{
		(Elem<N>::set(tuple.get<N>()), ...);
	}

	// Move constructor/assignment.
	constexpr TupleImpl(Self&& tuple) noexcept : TupleElem<N, T>{bzd::move(tuple.get<N>())}... {}
	constexpr void operator=(Self&& tuple) noexcept
	{
		(Elem<N>::set(bzd::move(tuple.get<N>())), ...);
	}

	// Access by index as template (type is automatically deducted)

	template <SizeType M>
	constexpr Pick<M>& get() noexcept
	{
		return Elem<M>::get();
	}

	template <SizeType M>
	constexpr const Pick<M>& get() const noexcept
	{
		return Elem<M>::get();
	}
};
} // namespace bzd::impl

namespace bzd {
template <class... T>
class Tuple : public impl::TupleImpl<impl::TupleRange<sizeof...(T)>, T...>
{
public:
	using impl::TupleImpl<impl::TupleRange<sizeof...(T)>, T...>::TupleImpl;

	static constexpr SizeType size() noexcept { return sizeof...(T); }
};
} // namespace bzd
