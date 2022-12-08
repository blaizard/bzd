#pragma once

#include "cc/bzd/algorithm/byte_copy.hh"
#include "cc/bzd/container/span.hh"
#include "cc/bzd/core/serialization/serialization.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/is_floating_point.hh"
#include "cc/bzd/type_traits/is_same.hh"
#include "cc/bzd/type_traits/remove_cvref.hh"
#include "cc/bzd/utility/numeric_limits.hh"

namespace bzd {

template <concepts::floatingPoint T>
struct Serialization<T>
{
	template <concepts::outputStream Range>
	static constexpr auto serialize(Range&& range, const T& value) noexcept
	{
		using Type = typeTraits::RemoveCVRef<T>;

		if constexpr (concepts::sameAs<Type, Float32> || concepts::sameAs<Type, Float64>)
		{
			static_assert(NumericLimits<Type>::isIEC559(),
						  "The floating point representation is not compliant with the IEC 559/IEEE 754 standard.");
			const auto bytes = Span<const T>{&value, 1u}.asBytes();
			return algorithm::byteCopy(bytes | impl::serialization::normalizeByteOrder(), range);
		}
		else
		{
			static_assert(bzd::meta::alwaysFalse<T>, "This type is not a supported floating point type.");
		}
	}
};

} // namespace bzd
