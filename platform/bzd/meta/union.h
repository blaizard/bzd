#pragma once

#include "bzd/type_traits/enable_if.h"
#include "bzd/type_traits/is_same.h"
#include "bzd/utility/forward.h"

namespace bzd { namespace meta {
namespace impl {
template <class T, class... Ts>
union UnionConstexpr {
public:
	// By default initialize the dummy element only, a constexpr constructor must
	// initialize something
	constexpr UnionConstexpr() : next_{} {}

	template <class U, typeTraits::EnableIf<!bzd::typeTraits::isSame<T, U>::value>* = nullptr>
	constexpr UnionConstexpr(U&& value) : next_{bzd::forward<U>(value)}
	{
	}
	template <class U, typeTraits::EnableIf<bzd::typeTraits::isSame<T, U>::value>* = nullptr>
	constexpr UnionConstexpr(const U& value) : value_{value}
	{
	}

	template <class U, typeTraits::EnableIf<!bzd::typeTraits::isSame<T, U>::value>* = nullptr>
	constexpr inline U& get()
	{
		return next_.template get<U>();
	}

	template <class U, typeTraits::EnableIf<!bzd::typeTraits::isSame<T, U>::value>* = nullptr>
	constexpr inline const U& get() const
	{
		return next_.template get<U>();
	}

	template <class U, typeTraits::EnableIf<bzd::typeTraits::isSame<T, U>::value>* = nullptr>
	constexpr inline U& get()
	{
		return value_;
	}

	template <class U, typeTraits::EnableIf<bzd::typeTraits::isSame<T, U>::value>* = nullptr>
	constexpr inline const U& get() const
	{
		return value_;
	}

protected:
	T value_;
	UnionConstexpr<Ts...> next_;
};

template <class T, class... Ts>
union Union {
public:
	// By default initialize the dummy element only, a constexpr constructor must
	// initialize something
	constexpr Union() : next_{} {}
	~Union(){}; // This is the only difference with a constexpr Union

	template <class U, typeTraits::EnableIf<!bzd::typeTraits::isSame<T, U>::value>* = nullptr>
	constexpr Union(U&& value) : next_{bzd::forward<U>(value)}
	{
	}
	template <class U, typeTraits::EnableIf<bzd::typeTraits::isSame<T, U>::value>* = nullptr>
	constexpr Union(const U& value) : value_{value}
	{
	}

	template <class U, typeTraits::EnableIf<!bzd::typeTraits::isSame<T, U>::value>* = nullptr>
	constexpr inline U& get()
	{
		return next_.template get<U>();
	}

	template <class U, typeTraits::EnableIf<!bzd::typeTraits::isSame<T, U>::value>* = nullptr>
	constexpr inline const U& get() const
	{
		return next_.template get<U>();
	}

	template <class U, typeTraits::EnableIf<bzd::typeTraits::isSame<T, U>::value>* = nullptr>
	constexpr inline U& get()
	{
		return value_;
	}

	template <class U, typeTraits::EnableIf<bzd::typeTraits::isSame<T, U>::value>* = nullptr>
	constexpr inline const U& get() const
	{
		return value_;
	}

protected:
	T value_;
	Union<Ts...> next_;
};

template <class T>
union UnionConstexpr<T> {
public:
	constexpr UnionConstexpr() : dummy_{} {}

	template <class U, typeTraits::EnableIf<bzd::typeTraits::isSame<T, U>::value>* = nullptr>
	constexpr UnionConstexpr(const U& value) : value_{value}
	{
	}

	template <class U, typeTraits::EnableIf<bzd::typeTraits::isSame<T, U>::value>* = nullptr>
	constexpr inline U& get()
	{
		return value_;
	}

	template <class U, typeTraits::EnableIf<bzd::typeTraits::isSame<T, U>::value>* = nullptr>
	constexpr inline const U& get() const
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
union Union<T> {
public:
	constexpr Union() : dummy_{} {}
	~Union() {}

	template <class U, typeTraits::EnableIf<bzd::typeTraits::isSame<T, U>::value>* = nullptr>
	constexpr Union(const U& value) : value_{value}
	{
	}

	template <class U, typeTraits::EnableIf<bzd::typeTraits::isSame<T, U>::value>* = nullptr>
	constexpr inline U& get()
	{
		return value_;
	}

	template <class U, typeTraits::EnableIf<bzd::typeTraits::isSame<T, U>::value>* = nullptr>
	constexpr inline const U& get() const
	{
		return value_;
	}

protected:
	T value_;
	struct
	{
	} dummy_;
};
} // namespace impl

template <class... Ts>
using Union = impl::Union<Ts...>;
template <class... Ts>
using UnionConstexpr = impl::UnionConstexpr<Ts...>;
}} // namespace bzd::meta
