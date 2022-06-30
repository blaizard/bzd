#pragma once

#include "cc/bzd/meta/range.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/remove_reference.hh"
#include "cc/bzd/utility/forward.hh"

namespace bzd::impl {

template <class F, class Tuple, bzd::Size... I>
constexpr decltype(auto) apply(F&& f, Tuple&& tuple, bzd::meta::range::Type<I...>)
{
	return bzd::forward<F>(f)(tuple.template get<I>()...);
}

} // namespace bzd::impl

namespace bzd {
/// Invoke the Callable object f with a tuple of arguments.
///
/// \param f Callable object to be invoked.
/// \param tuple whose elements to be used as arguments to f.
/// \return The value returned by f.
template <class F, class Tuple>
constexpr decltype(auto) apply(F&& f, Tuple&& tuple)
{
	return impl::apply(bzd::forward<F>(f), bzd::forward<Tuple>(tuple), bzd::meta::Range<0, typeTraits::RemoveReference<Tuple>::size()>{});
}
} // namespace bzd
