#pragma once

#include "cc/bzd/type_traits/is_convertible.hh"

namespace bzd::concepts {
template <class T, class U>
concept weaklyEqualityComparableWith = requires(const T& t, const U& u) {
										   {
											   t == u
											   } -> convertible<bool>;
										   {
											   t != u
											   } -> convertible<bool>;
										   {
											   u == t
											   } -> convertible<bool>;
										   {
											   u != t
											   } -> convertible<bool>;
									   };
} // namespace bzd::concepts
