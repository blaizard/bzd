#pragma once

#include "cc/bzd/type_traits/conditional.hh"
#include "cc/bzd/type_traits/enable_if.hh"
#include "cc/bzd/type_traits/first_type.hh"
#include "cc/bzd/type_traits/is_same.hh"
#include "cc/bzd/type_traits/is_trivially_destructible.hh"
#include "cc/bzd/utility/forward.hh"

namespace bzd::meta::impl {
template <class T, class... Ts>
union UnionTrivial {
public:
	// By default initialize the dummy element only, a constexpr constructor must
	// initialize something
	constexpr UnionTrivial() : next_{} {}

	template <class U, typeTraits::EnableIf<!bzd::typeTraits::isSame<T, U>>* = nullptr>
	constexpr UnionTrivial(U&& value) : next_{bzd::forward<U>(value)}
	{
	}
	template <class U, typeTraits::EnableIf<bzd::typeTraits::isSame<T, U>>* = nullptr>
	constexpr UnionTrivial(U&& value) : value_{bzd::forward<U>(value)}
	{
	}

	template <class U, typeTraits::EnableIf<!bzd::typeTraits::isSame<T, U>>* = nullptr>
	constexpr U& get()
	{
		return next_.template get<U>();
	}

	template <class U, typeTraits::EnableIf<!bzd::typeTraits::isSame<T, U>>* = nullptr>
	constexpr const U& get() const
	{
		return next_.template get<U>();
	}

	template <class U, typeTraits::EnableIf<bzd::typeTraits::isSame<T, U>>* = nullptr>
	constexpr U& get()
	{
		return value_;
	}

	template <class U, typeTraits::EnableIf<bzd::typeTraits::isSame<T, U>>* = nullptr>
	constexpr const U& get() const
	{
		return value_;
	}

protected:
	T value_;
	UnionTrivial<Ts...> next_;
};

template <class T, class... Ts>
union UnionNonTrivial {
public:
	// By default initialize the dummy element only, a constexpr constructor must
	// initialize something
	constexpr UnionNonTrivial() : next_{} {}

	// User-provided destructor is needed when T has non-trivial dtor.
	// This is the only difference with a UnionTrivial
	~UnionNonTrivial() {}

	template <class U, typeTraits::EnableIf<!bzd::typeTraits::isSame<T, U>>* = nullptr>
	constexpr UnionNonTrivial(U&& value) : next_{bzd::forward<U>(value)}
	{
	}
	template <class U, typeTraits::EnableIf<bzd::typeTraits::isSame<T, U>>* = nullptr>
	constexpr UnionNonTrivial(U&& value) : value_{bzd::forward<U>(value)}
	{
	}

	template <class U, typeTraits::EnableIf<!bzd::typeTraits::isSame<T, U>>* = nullptr>
	constexpr U& get()
	{
		return next_.template get<U>();
	}

	template <class U, typeTraits::EnableIf<!bzd::typeTraits::isSame<T, U>>* = nullptr>
	constexpr const U& get() const
	{
		return next_.template get<U>();
	}

	template <class U, typeTraits::EnableIf<bzd::typeTraits::isSame<T, U>>* = nullptr>
	constexpr U& get()
	{
		return value_;
	}

	template <class U, typeTraits::EnableIf<bzd::typeTraits::isSame<T, U>>* = nullptr>
	constexpr const U& get() const
	{
		return value_;
	}

protected:
	T value_;
	UnionNonTrivial<Ts...> next_;
};

template <class T>
union UnionTrivial<T> {
public:
	constexpr UnionTrivial() : dummy_{} {}

	template <class U, typeTraits::EnableIf<bzd::typeTraits::isSame<T, U>>* = nullptr>
	constexpr UnionTrivial(const U& value) : value_{value}
	{
	}

	template <class U, typeTraits::EnableIf<bzd::typeTraits::isSame<T, U>>* = nullptr>
	constexpr U& get()
	{
		return value_;
	}

	template <class U, typeTraits::EnableIf<bzd::typeTraits::isSame<T, U>>* = nullptr>
	constexpr const U& get() const
	{
		return value_;
	}

protected:
	T value_;
	struct
	{
	} dummy_;
};

template <class T>
union UnionNonTrivial<T> {
public:
	constexpr UnionNonTrivial() : dummy_{} {}
	~UnionNonTrivial() {}

	template <class U, typeTraits::EnableIf<bzd::typeTraits::isSame<T, U>>* = nullptr>
	constexpr UnionNonTrivial(const U& value) : value_{value}
	{
	}

	template <class U, typeTraits::EnableIf<bzd::typeTraits::isSame<T, U>>* = nullptr>
	constexpr U& get()
	{
		return value_;
	}

	template <class U, typeTraits::EnableIf<bzd::typeTraits::isSame<T, U>>* = nullptr>
	constexpr const U& get() const
	{
		return value_;
	}

protected:
	T value_;
	struct
	{
	} dummy_;
};
} // namespace bzd::meta::impl

namespace bzd::meta {

template <class... Ts>
using Union = bzd::typeTraits::
	Conditional<(bzd::typeTraits::isTriviallyDestructible<Ts> && ...), impl::UnionTrivial<Ts...>, impl::UnionNonTrivial<Ts...>>;
} // namespace bzd::meta
