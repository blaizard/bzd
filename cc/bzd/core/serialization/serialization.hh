#pragma once

#include "cc/bzd/container/range/views/all.hh"
#include "cc/bzd/container/range/views/reverse.hh"
#include "cc/bzd/meta/always_false.hh"
#include "cc/bzd/type_traits/container.hh"
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
/// \param container The container to be written to.
/// \param value The value to be written.
/// \return The number of bytes written. This number can be checked against
/// the available size in the container to see if the full serialization was written.
/// This design is used over a Result<> type for speed concerns.
template <bzd::concepts::containerToString Container, class T>
constexpr Size serialize(Container& container, const T& value) noexcept
{
	bzd::ignore = container;
	bzd::ignore = value;
	static_assert(bzd::meta::alwaysFalse<T>, "This type has no serializer.");
	return 0u;
}

} // namespace bzd
