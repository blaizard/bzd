#pragma once

#include "cc/bzd/container/array.hh"
#include "cc/bzd/meta/choose_nth.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/enable_if.hh"
#include "cc/bzd/type_traits/is_same.hh"
#include "cc/bzd/utility/forward.hh"
#include "cc/bzd/utility/in_place.hh"
#include "cc/bzd/utility/iterators/tuple.hh"
#include "cc/bzd/utility/move.hh"

namespace bzd::impl {
template <class T>
using TupleTypeOf = typename T::type;

template <Size... indexes>
struct TupleSizes : bzd::meta::Type<TupleSizes<indexes...>>
{
};

// given L>=0, generate sequence <0, ..., L-1>

template <Size L, Size I = 0, class S = TupleSizes<>>
struct TupleRangeImpl;

template <Size L, Size I, Size... indexes>
struct TupleRangeImpl<L, I, TupleSizes<indexes...>> : TupleRangeImpl<L, I + 1, TupleSizes<indexes..., I>>
{
};

template <Size L, Size... indexes>
struct TupleRangeImpl<L, L, TupleSizes<indexes...>> : TupleSizes<indexes...>
{
};

template <Size L>
using TupleRange = TupleTypeOf<TupleRangeImpl<L>>;

// tupleChooseN

struct NoType
{
};

template <Size Index>
constexpr NoType tupleChooseN()
{
	return NoType{};
}

template <Size Index, class T, class... Ts>
requires(Index > sizeof...(Ts))
constexpr NoType tupleChooseN(T&&, Ts&&...)
{
	return NoType{};
}

template <Size Index, class T, class... Ts>
requires(Index == 0)
constexpr decltype(auto) tupleChooseN(T&& t, Ts&&...)
{
	return bzd::forward<T>(t);
}

template <Size Index, class T, class... Ts>
requires(Index > 0 && Index <= sizeof...(Ts))
constexpr decltype(auto) tupleChooseN(T&&, Ts&&... ts)
{
	return tupleChooseN<Index - 1>(bzd::forward<Ts>(ts)...);
}

// single tuple element

template <Size index, class T>
class TupleElem
{
public:
	constexpr TupleElem() noexcept = default;
	constexpr TupleElem(const T& value) noexcept : elem_(value) {}
	template <class Value>
	requires(!concepts::sameAs<Value, NoType>)
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

template <class index, class... Ts>
class TupleImpl;

template <Size... indexes, class... Ts>
class TupleImpl<TupleSizes<indexes...>, Ts...> : TupleElem<indexes, Ts>...
{
private:
	using Self = TupleImpl<TupleSizes<indexes...>, Ts...>;
	template <Size index>
	using Pick = bzd::meta::ChooseNth<index, Ts...>;
	template <Size index>
	using Elem = TupleElem<index, Pick<index>>;

public:
	template <Size index>
	using ItemType = Pick<index>;

public: // constructors
	constexpr TupleImpl() noexcept = default;

	template <class... Args>
	constexpr TupleImpl(InPlace, Args&&... args) noexcept :
		TupleElem<indexes, Ts>{tupleChooseN<indexes, Args...>(bzd::forward<Args>(args)...)}...
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

// Add support for structure bindings: `auto [a, b] = bzd::Tuple<int, float>{1, 2.3};`
#include <tuple>

namespace std {
template <class... Args>
struct tuple_size<::bzd::Tuple<Args...>> : std::integral_constant<size_t, sizeof...(Args)>
{
};
template <size_t index, class... Args>
struct tuple_element<index, ::bzd::Tuple<Args...>>
{
	using type = typename ::bzd::meta::ChooseNth<index, Args...>;
};
} // namespace std
