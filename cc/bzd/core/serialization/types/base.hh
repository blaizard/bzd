#pragma once

#include "cc/bzd/container/optional.hh"
#include "cc/bzd/container/range/stream.hh"
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

namespace bzd {

/// This is the serialization base template and is used for partial or full specialization.
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
/// struct Serialization<T>
/// {
///     template <concepts::outputByteCopyableRange Range>
///     static constexpr Optional<Size> serialize(Range&& range, const T& value) noexcept { ... }
///
///     template <concepts::inputByteCopyableRange Range>
///     static constexpr Optional<Size> deserialize(Range&& range, T& value) noexcept { ... }
/// };
/// \endcode
template <class... Args>
struct Serialization
{
	static_assert(bzd::meta::alwaysFalse<Args...>, "This type has no serialization specialization.");
};

/// Serialize the value of a data type into an output range.
///
/// \param range The output range to be written to.
/// \param args The value(s) to be written.
/// \return The number of bytes written or nullopt in case of error (buffer is too small for example).
template <concepts::outputByteCopyableRange Range, class... Args>
constexpr Optional<Size> serialize(Range&& range, Args&&... args) noexcept
{
	return Serialization<Args...>::serialize(bzd::forward<Range>(range), bzd::forward<Args>(args)...);
}

/// Deserialize the value of a data type from a byte stream.
///
/// \param range The input range to be read from.
/// \param args The value(s) to be read.
/// \return The number of bytes read in case of success, otherwise an empty result.
template <concepts::inputByteCopyableRange Range, class... Args>
constexpr Optional<Size> deserialize(Range&& range, Args&&... args) noexcept
{
	return Serialization<Args...>::deserialize(bzd::forward<Range>(range), bzd::forward<Args>(args)...);
}

} // namespace bzd
