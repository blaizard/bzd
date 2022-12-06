#pragma once

#include "cc/bzd/core/serialization/serialization.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/is_integral.hh"
#include "cc/bzd/type_traits/is_same.hh"
#include "cc/bzd/type_traits/remove_cvref.hh"

namespace bzd {

template <bzd::concepts::containerToString Container, bzd::concepts::integral T>
constexpr Size serialize(Container& container, const T& value) noexcept
{
	using Type = typeTraits::RemoveCVRef<T>;

	if constexpr (concepts::sameAs<Type, bool> || bzd::concepts::sameAs<Type, Bool>)
	{
		container.append((value) ? bzd::Byte{1u} : bzd::Byte{0u});
		return 1u;
	}
	else if constexpr (concepts::sameAs<Type, Int8> || bzd::concepts::sameAs<Type, UInt8> || concepts::sameAs<Type, Int16> ||
					   bzd::concepts::sameAs<Type, UInt16> || concepts::sameAs<Type, Int32> || bzd::concepts::sameAs<Type, UInt32> ||
					   concepts::sameAs<Type, Int64> || bzd::concepts::sameAs<Type, UInt64> || concepts::sameAs<Type, Byte> ||
					   bzd::concepts::sameAs<Type, char>)
	{
		const auto bytes = bzd::Span<const T>{&value, 1u}.asBytes();
		container.append(bytes | impl::serialization::normalizeByteOrder());
		return bytes.size();
	}
	else
	{
		static_assert(bzd::meta::alwaysFalse<T>, "This type is not a supported integral type.");
	}
}

} // namespace bzd
