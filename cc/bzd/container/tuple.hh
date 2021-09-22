#pragma once

#include "cc/bzd/meta/choose_nth.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/enable_if.hh"
#include "cc/bzd/type_traits/is_same.hh"
#include "cc/bzd/utility/forward.hh"
#include "cc/bzd/utility/move.hh"

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
constexpr NoType TupleChooseN(T&&, Ts&&...)
{
	return NoType{};
}

template <SizeType Index, class T, class... Ts, typeTraits::EnableIf<Index == 0>* = nullptr>
constexpr decltype(auto) TupleChooseN(T&& t, Ts&&...)
{
	return bzd::forward<T>(t);
}

template <SizeType Index, class T, class... Ts, typeTraits::EnableIf<(Index > 0 && Index <= sizeof...(Ts))>* = nullptr>
constexpr decltype(auto) TupleChooseN(T&&, Ts&&... ts)
{
	return TupleChooseN<Index - 1>(bzd::forward<Ts>(ts)...);
}

// single tuple element

template <SizeType N, class T>
class TupleElem
{
public:
	constexpr TupleElem() noexcept = default;
	constexpr TupleElem(const T& value) noexcept : elem_(value) {}
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
	T elem_{};
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
	template <SizeType M>
	using ItemType = Pick<M>;

public: // constructors
	constexpr TupleImpl() noexcept = default;

	template <class... Args>
	constexpr TupleImpl(Args&&... args) noexcept : TupleElem<N, T>{TupleChooseN<N, Args...>(bzd::forward<Args>(args)...)}...
	{
	}

	// Copy/move constructor/assignment.
	constexpr TupleImpl(const Self& tuple) noexcept = default;
	constexpr Self& operator=(const Self& tuple) noexcept = default;
	constexpr TupleImpl(Self&& tuple) noexcept = default;
	constexpr Self& operator=(Self&& tuple) noexcept = default;
	~TupleImpl() noexcept = default;

public: // API
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

template <class... Args>
constexpr auto makeTuple(Args&&... args) noexcept
{
	return bzd::Tuple<Args...>{bzd::forward<Args>(args)...};
}

} // namespace bzd
