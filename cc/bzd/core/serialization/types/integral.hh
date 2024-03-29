#pragma once

#include "cc/bzd/algorithm/byte_copy.hh"
#include "cc/bzd/container/span.hh"
#include "cc/bzd/core/serialization/types/base.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/is_integral.hh"
#include "cc/bzd/type_traits/is_same.hh"
#include "cc/bzd/type_traits/remove_cvref.hh"

namespace bzd {

template <concepts::integral T>
struct Serialization<T>
{
	using Type = typeTraits::RemoveCVRef<T>;
	static constexpr Byte boolValueTrue{1u};
	static constexpr Byte boolValueFalse{0u};

	template <concepts::outputByteCopyableRange Range>
	static constexpr Optional<Size> serialize(Range&& range, const Type& value) noexcept
	requires(concepts::sameAs<Type, bool> || concepts::sameAs<Type, Bool>)
	{
		const auto data = {(value) ? boolValueTrue : boolValueFalse};
		if (algorithm::byteCopyReturnSize(data, range) == 1u)
		{
			return 1u;
		}
		return bzd::nullopt;
	}

	template <concepts::outputByteCopyableRange Range>
	static constexpr Optional<Size> serialize(Range&& range, const Type& value) noexcept
	requires(concepts::sameAs<Type, Int8> || concepts::sameAs<Type, UInt8> || concepts::sameAs<Type, Int16> ||
			 concepts::sameAs<Type, UInt16> || concepts::sameAs<Type, Int32> || concepts::sameAs<Type, UInt32> ||
			 concepts::sameAs<Type, Int64> || concepts::sameAs<Type, UInt64> || concepts::sameAs<Type, Byte> ||
			 concepts::sameAs<Type, char>)
	{
		const auto bytes = Span<const Type>{&value, 1u}.asBytes();
		const auto view = bytes | impl::serialization::normalizeByteOrder();
		if (algorithm::byteCopyReturnSize(view, range) == bytes.size())
		{
			return bytes.size();
		}
		return bzd::nullopt;
	}

	template <concepts::inputByteCopyableRange Range>
	static constexpr Optional<Size> deserialize(Range&& range, Type& value) noexcept
	requires(concepts::sameAs<Type, bool> || concepts::sameAs<Type, Bool>)
	{
		Byte bytes[1u];
		if (algorithm::byteCopyReturnSize(range, bytes) == 1u)
		{
			if (bytes[0] == boolValueTrue)
			{
				value = true;
				return 1u;
			}
			else if (bytes[0] == boolValueFalse)
			{
				value = false;
				return 1u;
			}
		}
		return bzd::nullopt;
	}

	template <concepts::inputByteCopyableRange Range>
	static constexpr Optional<Size> deserialize(Range&& range, Type& value) noexcept
	requires(concepts::sameAs<Type, Int8> || concepts::sameAs<Type, UInt8> || concepts::sameAs<Type, Int16> ||
			 concepts::sameAs<Type, UInt16> || concepts::sameAs<Type, Int32> || concepts::sameAs<Type, UInt32> ||
			 concepts::sameAs<Type, Int64> || concepts::sameAs<Type, UInt64> || concepts::sameAs<Type, Byte> ||
			 concepts::sameAs<Type, char>)
	{
		const auto bytes = Span<Type>{&value, 1u}.asBytesMutable();
		const auto view = range | impl::serialization::normalizeByteOrder();
		if (algorithm::byteCopyReturnSize(view, bytes) == bytes.size())
		{
			return bytes.size();
		}
		return bzd::nullopt;
	}
};

} // namespace bzd
