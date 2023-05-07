#pragma once

#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/range/stream.hh"
#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/meta/always_false.hh"
#include "cc/bzd/type_traits/is_same_class.hh"
#include "cc/bzd/type_traits/range.hh"

namespace bzd {

/// This is the matcher base template and is used for partial or full specialization.
template <class... Args>
struct Matcher
{
	static_assert(bzd::meta::alwaysFalse<Args...>, "This type has no matcher specialization.");
};

namespace typeTraits {

/// Match the Matcher specialization.
template <class T, class... Ts>
struct Matcher : ::bzd::Matcher<typeTraits::RemoveCVRef<T>>
{
};

} // namespace typeTraits

/// Match the value of a data type from a byte stream.
///
/// \param range The input range to be read from.
/// \param args The value(s) to be read.
/// \return The number of bytes read in case of success, otherwise an empty result.
template <concepts::inputStreamRange Range, class... Args>
constexpr Optional<Size> fromString(Range&& range, Args&&... args) noexcept
{
	return ::bzd::typeTraits::Matcher<Args...>::fromString(bzd::forward<Range>(range), bzd::forward<Args>(args)...);
}

/// \copydoc fromString
/// Converts an input range into an input stream.
template <concepts::inputRange Range, class... Args>
requires(!concepts::inputStreamRange<Range>)
constexpr Optional<Size> fromString(Range&& range, Args&&... args) noexcept
{
	::bzd::range::Stream stream{range};
	return ::bzd::fromString(stream, bzd::forward<Args>(args)...);
}

} // namespace bzd

namespace bzd::concepts {

/// Checks if a specific specialization for fromString exists.
template <class Range, class... Args>
concept fromString = requires(Range&& range, Args&&... args) {
						 ::bzd::typeTraits::Matcher<Args...>::fromString(bzd::forward<Range>(range), bzd::forward<Args>(args)...);
					 };

} // namespace bzd::concepts
