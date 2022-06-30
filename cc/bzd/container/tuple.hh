#pragma once

#include "cc/bzd/container/array.hh"
#include "cc/bzd/container/iterator/tuple.hh"
#include "cc/bzd/meta/choose_nth.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/enable_if.hh"
#include "cc/bzd/type_traits/is_same.hh"
#include "cc/bzd/utility/forward.hh"
#include "cc/bzd/utility/in_place.hh"
#include "cc/bzd/utility/move.hh"

namespace bzd::impl {
template <class T>
using TupleTypeOf = typename T::type;

template <Size... N>
struct TupleSizes : bzd::meta::Type<TupleSizes<N...>>
{
};

// given L>=0, generate sequence <0, ..., L-1>

template <Size L, Size I = 0, class S = TupleSizes<>>
struct TupleRangeImpl;

template <Size L, Size I, Size... N>
struct TupleRangeImpl<L, I, TupleSizes<N...>> : TupleRangeImpl<L, I + 1, TupleSizes<N..., I>>
{
};

template <Size L, Size... N>
struct TupleRangeImpl<L, L, TupleSizes<N...>> : TupleSizes<N...>
{
};

template <Size L>
using TupleRange = TupleTypeOf<TupleRangeImpl<L>>;

// TupleChooseN

struct NoType
{
};

template <Size Index>
constexpr NoType TupleChooseN()
{
	return NoType{};
}

template <Size Index, class T, class... Ts, typeTraits::EnableIf<(Index > sizeof...(Ts))>* = nullptr>
constexpr NoType TupleChooseN(T&&, Ts&&...)
{
	return NoType{};
}

template <Size Index, class T, class... Ts, typeTraits::EnableIf<Index == 0>* = nullptr>
constexpr decltype(auto) TupleChooseN(T&& t, Ts&&...)
{
	return bzd::forward<T>(t);
}

template <Size Index, class T, class... Ts, typeTraits::EnableIf<(Index > 0 && Index <= sizeof...(Ts))>* = nullptr>
constexpr decltype(auto) TupleChooseN(T&&, Ts&&... ts)
{
	return TupleChooseN<Index - 1>(bzd::forward<Ts>(ts)...);
}

// single tuple element

template <Size N, class T>
class TupleElem
{
public:
	constexpr TupleElem() noexcept = default;
	constexpr TupleElem(const T& value) noexcept : elem_(value) {}
	template <class Value, typeTraits::EnableIf<!typeTraits::isSame<Value, NoType>>* = nullptr>
	// NOLINTNEXTLINE(bugprone-forwarding-reference-overload)
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

template <class N, class... Ts>
class TupleImpl;

template <Size... N, class... Ts>
class TupleImpl<TupleSizes<N...>, Ts...> : TupleElem<N, Ts>...
{
private:
	using Self = TupleImpl<TupleSizes<N...>, Ts...>;
	template <Size M>
	using Pick = bzd::meta::ChooseNth<M, Ts...>;
	template <Size M>
	using Elem = TupleElem<M, Pick<M>>;

public:
	template <Size M>
	using ItemType = Pick<M>;

public: // constructors
	constexpr TupleImpl() noexcept = default;

	template <class... Args>
	constexpr TupleImpl(InPlace, Args&&... args) noexcept : TupleElem<N, Ts>{TupleChooseN<N, Args...>(bzd::forward<Args>(args)...)}...
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
	template <Size M>
	constexpr Pick<M>& get() noexcept
	{
		return Elem<M>::get();
	}

	template <Size M>
	constexpr const Pick<M>& get() const noexcept
	{
		return Elem<M>::get();
	}

	/// Get the number of entry of this tuple.
	///
	/// \return The number of entry.
	static constexpr Size size() noexcept { return sizeof...(Ts); }

public: // Iterator.
	using ConstIterator = bzd::iterator::Tuple<const Self, const Ts...>;
	using Iterator = bzd::iterator::Tuple<Self, Ts...>;

	[[nodiscard]] constexpr Iterator begin() noexcept { return {*this}; }
	[[nodiscard]] constexpr Iterator end() noexcept
	{
		Iterator it{*this};
		it += size();
		return it;
	}

	[[nodiscard]] constexpr ConstIterator begin() const noexcept { return {*this}; }
	[[nodiscard]] constexpr ConstIterator end() const noexcept
	{
		ConstIterator it{*this};
		it += size();
		return it;
	}
};
} // namespace bzd::impl

namespace bzd {
template <class... Ts>
class Tuple : public impl::TupleImpl<impl::TupleRange<sizeof...(Ts)>, Ts...>
{
public:
	using impl::TupleImpl<impl::TupleRange<sizeof...(Ts)>, Ts...>::TupleImpl;
};

template <class... Args>
constexpr auto makeTuple(Args&&... args) noexcept
{
	return bzd::Tuple<Args...>{inPlace, bzd::forward<Args>(args)...};
}

} // namespace bzd
