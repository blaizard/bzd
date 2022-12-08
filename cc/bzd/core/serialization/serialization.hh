#pragma once

#include "cc/bzd/container/range/views/all.hh"
#include "cc/bzd/container/range/views/reverse.hh"
#include "cc/bzd/meta/always_false.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/bit/endian.hh"
#include "cc/bzd/utility/ignore.hh"

namespace bzd::impl::serialization {

/// To byte order
inline constexpr auto normalizeByteOrder() noexcept
{
	// The endianness of serialized data are little endian.
	if constexpr (bzd::Endian::native == bzd::Endian::little)
	{
		return bzd::range::all();
	}
	else
	{
		return bzd::range::reverse();
	}
}

} // namespace bzd::impl::serialization

namespace bzd::concepts {

template <class T>
concept outputStream = concepts::streamRange<T> && concepts::byteCopyableOuptutRange<T>;

}

namespace bzd {

/// Serialize the value of a data type into bytes.
///
/// \param range The output range to be written to.
/// \param value The value to be written.
/// \return TBD.
template <concepts::outputStream Range, class T>
constexpr auto serialize(Range&& range, const T& value) noexcept
{
	bzd::ignore = range;
	bzd::ignore = value;
	static_assert(meta::alwaysFalse<T>, "This type has no serializer.");
}

/*
template <concepts::containerFromString Container, class T>
constexpr bzd::Optional<range::SubRange<Container>> deserialize(const Container& container, T& value) noexcept
{
	bzd::ignore = container;
	bzd::ignore = value;
	static_assert(bzd::meta::alwaysFalse<T>, "This type has no deserializer.");
	return 0u;
}
*/

} // namespace bzd
