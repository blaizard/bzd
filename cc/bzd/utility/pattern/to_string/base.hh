#pragma once

#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/range/views/stream.hh"
#include "cc/bzd/container/string_view.hh"
#include "cc/bzd/meta/always_false.hh"
#include "cc/bzd/type_traits/is_same_class.hh"
#include "cc/bzd/type_traits/range.hh"

namespace bzd {

/// This is the ToString base template and is used for partial or full specialization.
template <class... Args>
struct ToString
{
	static_assert(bzd::meta::alwaysFalse<Args...>, "This type has no ToString specialization.");
};

namespace typeTraits {

/// Match the ToString specialization.
template <class T, class... Ts>
struct ToString : ::bzd::ToString<typeTraits::RemoveCVRef<T>>
{
};

} // namespace typeTraits

/// Format the value of a data type into a byte stream.
///
/// \param range The input range to be written to.
/// \param args The value(s) to be written.
/// \return The number of bytes written in case of success, otherwise an empty result.
template <concepts::outputByteCopyableRange Range, class... Args>
constexpr Optional<Size> toString(Range&& range, Args&&... args) noexcept
{
	return ::bzd::typeTraits::ToString<Args...>::process(bzd::forward<Range>(range), bzd::forward<Args>(args)...);
}

} // namespace bzd
