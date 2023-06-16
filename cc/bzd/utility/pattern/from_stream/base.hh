#pragma once

#include "cc/bzd/container/ranges/stream.hh"
#include "cc/bzd/core/async.hh"
#include "cc/bzd/meta/always_false.hh"
#include "cc/bzd/type_traits/channel.hh"

namespace bzd {

/// This is the FromStream base template and is used for partial or full specialization.
template <class... Args>
struct FromStream
{
	static_assert(bzd::meta::alwaysFalse<Args...>, "This type has no FromStream specialization.");
};

namespace typeTraits {

/// Match the FromStream specialization.
template <class T, class... Ts>
struct FromStream : ::bzd::FromStream<typeTraits::RemoveCVRef<T>>
{
};

} // namespace typeTraits

/// Match the value of a data type from a byte stream.
///
/// \param input The input to be read from.
/// \param args The value(s) to be read.
/// \return The number of bytes read in case of success, otherwise an empty result.
template <concepts::readerAsync Input, class... Args>
bzd::Async<Size> fromStream(Input&& input, Args&&... args) noexcept
{
	co_return co_await ::bzd::typeTraits::FromStream<Args...>::process(bzd::forward<Input>(input), bzd::forward<Args>(args)...);
}

} // namespace bzd
