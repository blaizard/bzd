#pragma once

#include "cc/bzd/container/range/views/all.hh"
#include "cc/bzd/container/range/views/reverse.hh"
#include "cc/bzd/meta/always_false.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/bit/endian.hh"
#include "cc/bzd/utility/ignore.hh"

namespace bzd {

namespace impl::serialization {

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

} // namespace impl::serialization

/// Serialize the value of a data type into bytes.
///
/// \param range The output range to be written to.
/// \param value The value to be written.
/// \return The number of bytes written. This number can be checked against
/// the available size in the container to see if the full serialization was written.
/// This design is used over a Result<> type for speed concerns.
template <concepts::outputRange Range, class T>
constexpr auto serialize(Range&& range, const T& value) noexcept
{
	bzd::ignore = range;
	bzd::ignore = value;
	static_assert(meta::alwaysFalse<T>, "This type has no serializer.");
}

/*
template <bzd::concepts::containerFromString Container, class T>
constexpr bzd::Optional<range::SubRange<Container>> deserialize(const Container& container, T& value) noexcept
{
	bzd::ignore = container;
	bzd::ignore = value;
	static_assert(bzd::meta::alwaysFalse<T>, "This type has no deserializer.");
	return 0u;
}
*/

} // namespace bzd
