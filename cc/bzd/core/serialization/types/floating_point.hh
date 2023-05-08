#pragma once

#include "cc/bzd/algorithm/byte_copy.hh"
#include "cc/bzd/container/span.hh"
#include "cc/bzd/core/serialization/types/base.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/is_floating_point.hh"
#include "cc/bzd/type_traits/is_same.hh"
#include "cc/bzd/type_traits/remove_cvref.hh"
#include "cc/bzd/utility/numeric_limits.hh"

namespace bzd {

template <concepts::floatingPoint T>
struct Serialization<T>
{
	using Type = typeTraits::RemoveCVRef<T>;

	static_assert(concepts::sameAs<Type, Float32> || concepts::sameAs<Type, Float64>, "This type is not a supported floating point type.");
	static_assert(NumericLimits<Type>::isIEC559(),
				  "The floating point representation is not compliant with the IEC 559/IEEE 754 standard.");

	template <concepts::outputStreamRange Range>
	static constexpr Optional<Size> serialize(Range&& range, const Type& value) noexcept
	{
		const auto bytes = Span<const Type>{&value, 1u}.asBytes();
		const auto view = bytes | impl::serialization::normalizeByteOrder();
		if (algorithm::byteCopyReturnSize(view, range) == bytes.size())
		{
			return bytes.size();
		}
		return bzd::nullopt;
	}

	template <concepts::inputStreamRange Range>
	static constexpr Optional<Size> deserialize(Range&& range, Type& value) noexcept
	{
		auto bytes = Span<Type>{&value, 1u}.asBytesMutable();
		const auto view = range | impl::serialization::normalizeByteOrder();
		if (algorithm::byteCopyReturnSize(view, bytes) == bytes.size())
		{
			return bytes.size();
		}
		return bzd::nullopt;
	}
};

} // namespace bzd
