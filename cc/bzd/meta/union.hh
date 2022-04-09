#pragma once

#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/conditional.hh"
#include "cc/bzd/type_traits/enable_if.hh"
#include "cc/bzd/type_traits/first_type.hh"
#include "cc/bzd/type_traits/is_same.hh"
#include "cc/bzd/type_traits/is_trivially_destructible.hh"
#include "cc/bzd/type_traits/remove_reference.hh"
#include "cc/bzd/utility/forward.hh"
#include "cc/bzd/utility/in_place.hh"

namespace bzd::meta::impl {

class UnionTag
{
};

template <SizeType Index, class T, class... Ts>
union UnionTrivial {
public: // Traits
	using Self = UnionTrivial<Index, T, Ts...>;

public:
	// By default initialize the dummy element only, a constexpr constructor must
	// initialize something
	constexpr UnionTrivial() noexcept : next_{} {}

	// Value constructor (copy/move)
	template <class U, typeTraits::EnableIf<!bzd::typeTraits::isSame<T, bzd::typeTraits::RemoveReference<U>>>* = nullptr>
	// NOLINTNEXTLINE(bugprone-forwarding-reference-overload)
	constexpr UnionTrivial(U&& value) noexcept : next_{bzd::forward<U>(value)}
	{
	}

	template <class U, typeTraits::EnableIf<bzd::typeTraits::isSame<T, bzd::typeTraits::RemoveReference<U>>>* = nullptr>
	// NOLINTNEXTLINE(bugprone-forwarding-reference-overload)
	constexpr UnionTrivial(U&& value) noexcept : value_{bzd::forward<U>(value)}
	{
	}

	template <SizeType I, class... Args, typeTraits::EnableIf<I != Index>* = nullptr>
	constexpr UnionTrivial(InPlaceIndex<I>, Args&&... args) noexcept : next_{inPlaceIndex<I>, bzd::forward<Args>(args)...}
	{
	}

	template <SizeType I, class... Args, typeTraits::EnableIf<I == Index>* = nullptr>
	constexpr UnionTrivial(InPlaceIndex<I>, Args&&... args) noexcept : value_{bzd::forward<Args>(args)...}
	{
	}

	template <class U, class... Args, typeTraits::EnableIf<!bzd::typeTraits::isSame<T, U>>* = nullptr>
	constexpr UnionTrivial(InPlaceType<U>, Args&&... args) noexcept : next_{inPlaceType<U>, bzd::forward<Args>(args)...}
	{
	}

	template <class U, class... Args, typeTraits::EnableIf<bzd::typeTraits::isSame<T, U>>* = nullptr>
	constexpr UnionTrivial(InPlaceType<U>, Args&&... args) noexcept : value_{bzd::forward<Args>(args)...}
	{
	}

	// Value assignments (copy/move).
	template <class U>
	constexpr Self& operator=(U&& value) noexcept
	{
		set<bzd::typeTraits::RemoveReference<U>>(bzd::forward<U>(value));
		return *this;
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

	template <class U, class V, typeTraits::EnableIf<!bzd::typeTraits::isSame<T, U>>* = nullptr>
	constexpr void set(V&& value) noexcept
	{
		next_.template set<U>(bzd::forward<V>(value));
	}

	template <class U, class V, typeTraits::EnableIf<bzd::typeTraits::isSame<T, U>>* = nullptr>
	constexpr void set(V&& value) noexcept
	{
		value_ = bzd::forward<V>(value);
	}

protected:
	T value_;
	UnionTrivial<Index + 1, Ts...> next_;
};

template <SizeType Index, class T, class... Ts>
union UnionNonTrivial {
public: // Traits
	using Self = UnionNonTrivial<Index, T, Ts...>;

public:
	// By default initialize the dummy element only, a constexpr constructor must
	// initialize something
	constexpr UnionNonTrivial() noexcept : next_{} {}

	// User-provided destructor is needed when T has non-trivial dtor.
	// This is the only difference with a UnionTrivial
	~UnionNonTrivial() noexcept {}

	// Value constructor (copy/move)
	template <class U, typeTraits::EnableIf<!bzd::typeTraits::isSame<T, bzd::typeTraits::RemoveReference<U>>>* = nullptr>
	// NOLINTNEXTLINE(bugprone-forwarding-reference-overload)
	constexpr UnionNonTrivial(U&& value) noexcept : next_{bzd::forward<U>(value)}
	{
	}

	template <class U, typeTraits::EnableIf<bzd::typeTraits::isSame<T, bzd::typeTraits::RemoveReference<U>>>* = nullptr>
	// NOLINTNEXTLINE(bugprone-forwarding-reference-overload)
	constexpr UnionNonTrivial(U&& value) noexcept : value_{bzd::forward<U>(value)}
	{
	}

	template <SizeType I, class... Args, typeTraits::EnableIf<I != Index>* = nullptr>
	constexpr UnionNonTrivial(InPlaceIndex<I>, Args&&... args) noexcept : next_{inPlaceIndex<I>, bzd::forward<Args>(args)...}
	{
	}

	template <SizeType I, class... Args, typeTraits::EnableIf<I == Index>* = nullptr>
	constexpr UnionNonTrivial(InPlaceIndex<I>, Args&&... args) noexcept : value_{bzd::forward<Args>(args)...}
	{
	}

	template <class U, class... Args, typeTraits::EnableIf<!bzd::typeTraits::isSame<T, U>>* = nullptr>
	constexpr UnionNonTrivial(InPlaceType<U>, Args&&... args) noexcept : next_{inPlaceType<U>, bzd::forward<Args>(args)...}
	{
	}

	template <class U, class... Args, typeTraits::EnableIf<bzd::typeTraits::isSame<T, U>>* = nullptr>
	constexpr UnionNonTrivial(InPlaceType<U>, Args&&... args) noexcept : value_{bzd::forward<Args>(args)...}
	{
	}

	// Value assignments (copy/move).
	template <class U>
	constexpr Self& operator=(U&& value) noexcept
	{
		set<bzd::typeTraits::RemoveReference<U>>(bzd::forward<U>(value));
		return *this;
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

	template <class U, class V, typeTraits::EnableIf<!bzd::typeTraits::isSame<T, U>>* = nullptr>
	constexpr void set(V&& value) noexcept
	{
		next_.template set<U>(bzd::forward<V>(value));
	}

	template <class U, class V, typeTraits::EnableIf<bzd::typeTraits::isSame<T, U>>* = nullptr>
	constexpr void set(V&& value) noexcept
	{
		value_ = bzd::forward<V>(value);
	}

protected:
	T value_;
	UnionNonTrivial<Index + 1, Ts...> next_;
};

template <SizeType Index>
union UnionTrivial<Index, UnionTag> {
};

template <SizeType Index>
union UnionNonTrivial<Index, UnionTag> {
};
} // namespace bzd::meta::impl

namespace bzd::meta {

template <class... Ts>
using Union = bzd::typeTraits::Conditional<(bzd::typeTraits::isTriviallyDestructible<Ts> && ...),
										   impl::UnionTrivial<0, Ts..., impl::UnionTag>,
										   impl::UnionNonTrivial<0, Ts..., impl::UnionTag>>;
} // namespace bzd::meta
