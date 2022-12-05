#pragma once

#include "cc/bzd/core/serialization/serialization.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/is_integral.hh"
#include "cc/bzd/type_traits/is_same.hh"
#include "cc/bzd/type_traits/remove_cvref.hh"

namespace bzd {

template <bzd::concepts::containerToString Container, bzd::concepts::integral T>
constexpr Size serialize(Container& str, const T& value) noexcept
{
	if constexpr (concepts::sameAs<typeTraits::RemoveCVRef<T>, bool> || bzd::concepts::sameAs<typeTraits::RemoveCVRef<T>, Bool>)
	{
		str.append((value) ? bzd::Byte{1u} : bzd::Byte{0u});
		return 1u;
	}
	else
	{
        // TODO: to implement.
		return 0u;
	}
}

} // namespace bzd
