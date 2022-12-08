#pragma once

#include "cc/bzd/algorithm/byte_copy.hh"
#include "cc/bzd/container/span.hh"
#include "cc/bzd/core/serialization/serialization.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/is_integral.hh"
#include "cc/bzd/type_traits/is_same.hh"
#include "cc/bzd/type_traits/remove_cvref.hh"

namespace bzd {

template <concepts::outputStream Range, concepts::integral T>
constexpr auto serialize(Range&& range, const T& value) noexcept
{
	using Type = typeTraits::RemoveCVRef<T>;

	if constexpr (concepts::sameAs<Type, bool> || concepts::sameAs<Type, Bool>)
	{
		const auto data = {(value) ? Byte{1u} : Byte{0u}};
		return algorithm::byteCopy(data, range);
	}
	else if constexpr (concepts::sameAs<Type, Int8> || concepts::sameAs<Type, UInt8> || concepts::sameAs<Type, Int16> ||
					   concepts::sameAs<Type, UInt16> || concepts::sameAs<Type, Int32> || concepts::sameAs<Type, UInt32> ||
					   concepts::sameAs<Type, Int64> || concepts::sameAs<Type, UInt64> || concepts::sameAs<Type, Byte> ||
					   concepts::sameAs<Type, char>)
	{
		const auto bytes = Span<const T>{&value, 1u}.asBytes();
		return algorithm::byteCopy(bytes | impl::serialization::normalizeByteOrder(), range);
	}
	else
	{
		static_assert(meta::alwaysFalse<T>, "This type is not a supported integral type.");
	}
}

} // namespace bzd
