#pragma once

#include "cc/bzd/type_traits/conditional.hh"
#include "cc/bzd/type_traits/enable_if.hh"
#include "cc/bzd/type_traits/first_type.hh"
#include "cc/bzd/type_traits/is_same.hh"
#include "cc/bzd/type_traits/is_trivially_destructible.hh"
#include "cc/bzd/utility/forward.hh"

#include <type_traits>

namespace bzd::meta::impl {

class UnionTag
{
};

template <class T, class... Ts>
union UnionTrivial {
public:
	// By default initialize the dummy element only, a constexpr constructor must
	// initialize something
	constexpr UnionTrivial() noexcept : next_{} {}

	template <class U, typeTraits::EnableIf<!bzd::typeTraits::isSame<T, U>>* = nullptr>
	constexpr UnionTrivial(U&& value) noexcept : next_{bzd::forward<U>(value)}
	{
	}

	template <class U, typeTraits::EnableIf<bzd::typeTraits::isSame<T, U>>* = nullptr>
	constexpr UnionTrivial(U&& value) noexcept : value_{bzd::forward<U>(value)}
	{
	}

	template <class U, typeTraits::EnableIf<!bzd::typeTraits::isSame<T, U>>* = nullptr>
	constexpr U& get() noexcept
	{
		return next_.template get<U>();
	}

	template <class U, typeTraits::EnableIf<!bzd::typeTraits::isSame<T, U>>* = nullptr>
	constexpr const U& get() const noexcept
	{
		return next_.template get<U>();
	}

	template <class U, typeTraits::EnableIf<bzd::typeTraits::isSame<T, U>>* = nullptr>
	constexpr U& get() noexcept
	{
		return value_;
	}

	template <class U, typeTraits::EnableIf<bzd::typeTraits::isSame<T, U>>* = nullptr>
	constexpr const U& get() const noexcept
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
	constexpr UnionNonTrivial() noexcept : next_{} {}

	// User-provided destructor is needed when T has non-trivial dtor.
	// This is the only difference with a UnionTrivial
	~UnionNonTrivial() noexcept {}

	template <class U, typeTraits::EnableIf<!bzd::typeTraits::isSame<T, U>>* = nullptr>
	constexpr UnionNonTrivial(U&& value) noexcept : next_{bzd::forward<U>(value)}
	{
	}
	template <class U, typeTraits::EnableIf<bzd::typeTraits::isSame<T, U>>* = nullptr>
	constexpr UnionNonTrivial(U&& value) noexcept : value_{bzd::forward<U>(value)}
	{
	}

	template <class U, typeTraits::EnableIf<!bzd::typeTraits::isSame<T, U>>* = nullptr>
	constexpr U& get() noexcept
	{
		return next_.template get<U>();
	}

	template <class U, typeTraits::EnableIf<!bzd::typeTraits::isSame<T, U>>* = nullptr>
	constexpr const U& get() const noexcept
	{
		return next_.template get<U>();
	}

	template <class U, typeTraits::EnableIf<bzd::typeTraits::isSame<T, U>>* = nullptr>
	constexpr U& get() noexcept
	{
		return value_;
	}

	template <class U, typeTraits::EnableIf<bzd::typeTraits::isSame<T, U>>* = nullptr>
	constexpr const U& get() const noexcept
	{
		return value_;
	}

protected:
	T value_;
	UnionNonTrivial<Ts...> next_;
};

template <>
union UnionTrivial<UnionTag> {
};

template <>
union UnionNonTrivial<UnionTag> {
};
} // namespace bzd::meta::impl

namespace bzd::meta {

template <class... Ts>
using Union = bzd::typeTraits::Conditional<(bzd::typeTraits::isTriviallyDestructible<Ts> && ...),
										   impl::UnionTrivial<Ts..., impl::UnionTag>,
										   impl::UnionNonTrivial<Ts..., impl::UnionTag>>;
} // namespace bzd::meta
