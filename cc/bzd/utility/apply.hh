#pragma once

#include "cc/bzd/meta/range.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/remove_reference.hh"
#include "cc/bzd/utility/forward.hh"

namespace bzd::impl {

template <class F, class T, bzd::Size... i>
constexpr decltype(auto) apply(F&& f, T&& t, bzd::meta::range::Type<i...>)
{
	return bzd::forward<F>(f)(t.template get<i>()...);
}

} // namespace bzd::impl

namespace bzd {
/// Invoke the Callable object f with a fixed size container.
///
/// \param f Callable object to be invoked.
/// \param t whose elements to be used as arguments to f.
/// \return The value returned by f.
template <class F, class T>
constexpr decltype(auto) apply(F&& f, T&& t)
{
	return impl::apply(bzd::forward<F>(f), bzd::forward<T>(t), bzd::meta::Range<0, typeTraits::RemoveReference<T>::size()>{});
}
} // namespace bzd
