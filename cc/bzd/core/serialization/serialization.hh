#pragma once

#include "cc/bzd/meta/always_false.hh"
#include "cc/bzd/type_traits/container.hh"
#include "cc/bzd/utility/ignore.hh"

namespace bzd {

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
