#pragma once

#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/range/stream.hh"
#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/meta/always_false.hh"
#include "cc/bzd/type_traits/is_same_class.hh"
#include "cc/bzd/type_traits/range.hh"

namespace bzd {

/// This is the formatter base template and is used for partial or full specialization.
template <class... Args>
struct Formatter
{
	static_assert(bzd::meta::alwaysFalse<Args...>, "This type has no formatter specialization.");
};

namespace typeTraits {

/// Match the Formatter specialization.
template <class T, class... Ts>
struct Formatter : ::bzd::Formatter<typeTraits::RemoveCVRef<T>>
{
};

} // namespace typeTraits

/// Format the value of a data type into a byte stream.
///
/// \param range The input range to be written to.
/// \param args The value(s) to be written.
/// \return The number of bytes written in case of success, otherwise an empty result.
template <concepts::outputStreamRange Range, class... Args>
constexpr Optional<Size> toString(Range&& range, Args&&... args) noexcept
{
	return ::bzd::typeTraits::Formatter<Args...>::toString(bzd::forward<Range>(range), bzd::forward<Args>(args)...);
}

/// \copydoc toString
/// Converts an output range into an output stream.
template <concepts::outputRange Range, class... Args>
requires(!concepts::outputStreamRange<Range>)
constexpr Optional<Size> toString(Range&& range, Args&&... args) noexcept
{
	::bzd::range::Stream stream{range};
	return ::bzd::toString(stream, bzd::forward<Args>(args)...);
}

} // namespace bzd
