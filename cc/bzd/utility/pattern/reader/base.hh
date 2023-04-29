#pragma once

#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/range/stream.hh"
#include "cc/bzd/meta/always_false.hh"
#include "cc/bzd/type_traits/range.hh"

namespace bzd {

/// This is the formatter base template and is used for partial or full specialization.
///
/// Why such design?
/// The use of a class here is to allow partial specialization.
/// Also function overloading cannot be used here because the definition order will matter,
/// as those can be used in a recursive way. This would lead to error such as:
/// "specialization of [...] after instanciation".
/// Some article on the matter: http://www.gotw.ca/publications/mill17.htm
///
/// How to use it?
/// \code
/// template <concepts::??? T>
/// struct Formatter<T>
/// {
///     template <concepts::outputStreamRange Range>
///     static constexpr Optional<Size> toString(Range&& range, const T& value) noexcept { ... }
///
///     template <concepts::inputStreamRange Range>
///     static constexpr Optional<Size> fromString(Range&& range, T& value) noexcept { ... }
/// };
/// \endcode
template <class... Args>
struct Formatter;

/*
/// Format the value of a data type into a byte stream.
///
/// \param range The output range to be written to.
/// \param args The value(s) to be written.
/// \return The number of bytes written or nullopt in case of error (buffer is too small for example).
template <concepts::outputStreamRange Range, class... Args>
constexpr Optional<Size> toString(Range&& range, Args&&... args) noexcept
{
	return Formatter<Args...>::toString(bzd::forward<Range>(range), bzd::forward<Args>(args)...);
}

/// \copydoc toString
/// Converts an output range into an output stream.
template <concepts::outputRange Range, class... Args>
requires(!concepts::outputStreamRange<Range>)
constexpr Optional<Size> toString(Range&& range, Args&&... args) noexcept
{
	range::Stream stream{bzd::begin(range), bzd::end(range)};
	return bzd::toString(stream, bzd::forward<Args>(args)...);
}
*/
/// Match the value of a data type from a byte stream.
///
/// \param range The input range to be read from.
/// \param args The value(s) to be read.
/// \return The number of bytes read in case of success, otherwise an empty result.
template <concepts::inputStreamRange Range, class... Args>
constexpr Optional<Size> fromString(Range&& range, Args&&... args) noexcept
{
	return ::bzd::Formatter<typeTraits::RemoveCVRef<Args>...>::fromString(bzd::forward<Range>(range), bzd::forward<Args>(args)...);
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
concept fromStringSpecialization =
	requires(Range&& range, Args&&... args) {
		::bzd::Formatter<typeTraits::RemoveCVRef<Args>...>::fromString(bzd::forward<Range>(range), bzd::forward<Args>(args)...);
	};

} // namespace bzd::concepts
