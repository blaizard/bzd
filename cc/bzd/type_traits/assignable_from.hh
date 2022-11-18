#pragma once

#include "cc/bzd/type_traits/is_same.hh"
#include "cc/bzd/utility/forward.hh"

namespace bzd::concepts {

template <class T, class U>
concept assignableFrom = requires(T t, U&& u) {
							 {
								 t = bzd::forward<U>(u)
								 } -> bzd::concepts::sameAs<T>;
						 };

} // namespace bzd::concepts
