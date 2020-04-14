#pragma once

#include "bzd/meta/choose_nth.h"
#include "bzd/type_traits/enable_if.h"
#include "bzd/type_traits/is_same.h"
#include "bzd/types.h"
#include "bzd/utility/forward.h"

namespace bzd {
namespace impl {
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
	template <SizeType M>
	using pick = bzd::meta::ChooseNth<M, T...>;
	template <SizeType M>
	using elem = TupleElem<M, pick<M>>;

public:
	template <class... Args>
	constexpr TupleImpl(Args&&... args) noexcept : TupleElem<N, T>{TupleChooseN<N, Args...>(bzd::forward<Args>(args)...)}...
	{
	}

	// Access by index as template (type is automatically deducted)

	template <SizeType M>
	constexpr pick<M>& get() noexcept
	{
		return elem<M>::get();
	}

	template <SizeType M>
	constexpr const pick<M>& get() const noexcept
	{
		return elem<M>::get();
	}
};
} // namespace impl

template <class... T>
class Tuple : public impl::TupleImpl<impl::TupleRange<sizeof...(T)>, T...>
{
public:
	// Forward constructor as constexpr

	template <class... Args>
	constexpr Tuple(Args&&... args) noexcept : impl::TupleImpl<impl::TupleRange<sizeof...(T)>, T...>(bzd::forward<Args>(args)...)
	{
	}

	static constexpr SizeType size() noexcept { return sizeof...(T); }
};
} // namespace bzd
