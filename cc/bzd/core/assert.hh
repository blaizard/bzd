#pragma once

#include "cc/bzd/container/result.hh"
#include "cc/bzd/core/assert/minimal.hh"
#include "cc/bzd/core/logger.hh"
#include "cc/bzd/platform/panic.hh"
#include "cc/bzd/utility/source_location.hh"

namespace bzd::assert {
template <class A>
constexpr void isTrue(const bool condition, A&& a, const bzd::SourceLocation location = bzd::SourceLocation::current());
template <class A, class B>
constexpr void isTrue(const bool condition, A&& a, B&& b, const bzd::SourceLocation location = bzd::SourceLocation::current());
template <class A, class B, class C>
constexpr void isTrue(const bool condition, A&& a, B&& b, C&& c, const bzd::SourceLocation location = bzd::SourceLocation::current());
template <class A, class B, class C, class D>
constexpr void isTrue(
	const bool condition, A&& a, B&& b, C&& c, D&& d, const bzd::SourceLocation location = bzd::SourceLocation::current());
template <class A, class B, class C, class D, class E>
constexpr void isTrue(
	const bool condition, A&& a, B&& b, C&& c, D&& d, E&& e, const bzd::SourceLocation location = bzd::SourceLocation::current());
template <class A, class B, class C, class D, class E, class F>
constexpr void isTrue(
	const bool condition, A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, const bzd::SourceLocation location = bzd::SourceLocation::current());
template <class A, class B, class C, class D, class E, class F, class G>
constexpr void isTrue(const bool condition,
					  A&& a,
					  B&& b,
					  C&& c,
					  D&& d,
					  E&& e,
					  F&& f,
					  G&& g,
					  const bzd::SourceLocation location = bzd::SourceLocation::current());

template <class T, class E>
constexpr void isResult(const Result<T, E>& result, const bzd::SourceLocation location = bzd::SourceLocation::current());

} // namespace bzd::assert

// ----------------------------------------------------------------------------
// Implementation

template <class A>
constexpr void bzd::assert::isTrue(const bool condition, A&& a, const bzd::SourceLocation location)
{
	if (!condition)
	{
		bzd::log::error(bzd::forward<A>(a), location);
		bzd::platform::panic();
	}
}

template <class A, class B>
constexpr void bzd::assert::isTrue(const bool condition, A&& a, B&& b, const bzd::SourceLocation location)
{
	if (!condition)
	{
		bzd::log::error(bzd::forward<A>(a), bzd::forward<B>(b), location);
		bzd::platform::panic();
	}
}
template <class A, class B, class C>
constexpr void bzd::assert::isTrue(const bool condition, A&& a, B&& b, C&& c, const bzd::SourceLocation location)
{
	if (!condition)
	{
		bzd::log::error(bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c), location);
		bzd::platform::panic();
	}
}
template <class A, class B, class C, class D>
constexpr void bzd::assert::isTrue(const bool condition, A&& a, B&& b, C&& c, D&& d, const bzd::SourceLocation location)
{
	if (!condition)
	{
		bzd::log::error(bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c), bzd::forward<D>(d), location);
		bzd::platform::panic();
	}
}
template <class A, class B, class C, class D, class E>
constexpr void bzd::assert::isTrue(const bool condition, A&& a, B&& b, C&& c, D&& d, E&& e, const bzd::SourceLocation location)
{
	if (!condition)
	{
		bzd::log::error(bzd::forward<A>(a), bzd::forward<B>(b), bzd::forward<C>(c), bzd::forward<D>(d), bzd::forward<E>(e), location);
		bzd::platform::panic();
	}
}
template <class A, class B, class C, class D, class E, class F>
constexpr void bzd::assert::isTrue(const bool condition, A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, const bzd::SourceLocation location)
{
	if (!condition)
	{
		bzd::log::error(bzd::forward<A>(a),
						bzd::forward<B>(b),
						bzd::forward<C>(c),
						bzd::forward<D>(d),
						bzd::forward<E>(e),
						bzd::forward<F>(f),
						location);
		bzd::platform::panic();
	}
}
template <class A, class B, class C, class D, class E, class F, class G>
constexpr void bzd::assert::isTrue(
	const bool condition, A&& a, B&& b, C&& c, D&& d, E&& e, F&& f, G&& g, const bzd::SourceLocation location)
{
	if (!condition)
	{
		bzd::log::error(bzd::forward<A>(a),
						bzd::forward<B>(b),
						bzd::forward<C>(c),
						bzd::forward<D>(d),
						bzd::forward<E>(e),
						bzd::forward<F>(f),
						bzd::forward<G>(g),
						location);
		bzd::platform::panic();
	}
}

template <class T, class E>
constexpr void bzd::assert::isResult(const Result<T, E>& result, const bzd::SourceLocation location)
{
	if (!result)
	{
		bzd::assert::isTrue(false, "Result failed: {}"_csv, result.error(), location);
	}
}
