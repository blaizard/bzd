#pragma once

#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/range/views/stream.hh"
#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/meta/always_false.hh"
#include "cc/bzd/type_traits/is_same_class.hh"
#include "cc/bzd/type_traits/range.hh"

namespace bzd {

/// This is the FromString base template and is used for partial or full specialization.
template <class... Args>
struct FromString
{
	static_assert(bzd::meta::alwaysFalse<Args...>, "This type has no FromString specialization.");
};

namespace typeTraits {

/// Match the FromString specialization.
template <class T, class... Ts>
struct FromString : ::bzd::FromString<typeTraits::RemoveCVRef<T>>
{
};

} // namespace typeTraits

/// Match the value of a data type from a byte stream.
///
/// \param range The input range to be read from.
/// \param args The value(s) to be read.
/// \return The number of bytes read in case of success, otherwise an empty result.
template <concepts::inputByteCopyableRange Range, class... Args>
constexpr Optional<Size> fromString(Range&& range, Args&&... args) noexcept
{
	return ::bzd::typeTraits::FromString<Args...>::process(bzd::forward<Range>(range), bzd::forward<Args>(args)...);
}

} // namespace bzd
