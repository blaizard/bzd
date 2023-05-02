#pragma once

#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/range/stream.hh"
#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/meta/always_false.hh"
#include "cc/bzd/type_traits/is_same_class.hh"
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
/// struct Matcher<T>
/// {
///     template <concepts::inputStreamRange Range>
///     static constexpr Optional<Size> fromString(Range&& range, T& value) noexcept { ... }
/// };
/// \endcode
///
/// or with metadata:
/// \code
/// template <concepts::??? T>
/// struct Matcher<T>
/// {
///     struct Metadata {};
///
///     static constexpr Metadata metadata() noexcept { .... }
///
///     template <concepts::inputStreamRange Range>
///     static constexpr Optional<Size> fromString(Range&& range, T& value, const Metadata) noexcept { ... }
/// };
/// \endcode
template <class... Args>
struct Matcher
{
	static_assert(bzd::meta::alwaysFalse<Args...>, "This type has no matcher specialization.");
};

struct FromStringMetadata
{
	bzd::Size index{0u};
	bzd::StringView pattern{};
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

/// Checks if a Matcher specialization has metadata.
template <class... Args>
concept matcherMetadata = requires { typename ::bzd::typeTraits::template Matcher<Args...>::Metadata; };

} // namespace bzd::concepts
