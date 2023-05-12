#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/bzd/core/channel.hh"
#include "cc/bzd/meta/always_false.hh"

namespace bzd {

/// This is the formatter base template and is used for partial or full specialization.
template <class... Args>
struct ToStream
{
	static_assert(bzd::meta::alwaysFalse<Args...>, "This type has no ToStream specialization.");
};

namespace typeTraits {

/// Match the ToStream specialization.
template <class T, class... Ts>
struct ToStream : ::bzd::ToStream<typeTraits::RemoveCVRef<T>>
{
};

} // namespace typeTraits

/// Format the value of a data type into a byte stream.
///
/// \param stream The output stream to be written to.
/// \param args The value(s) to be written.
/// \return The number of bytes written in case of success, otherwise an empty result.
template <class... Args>
bzd::Async<> toStream(bzd::OStream& stream, Args&&... args) noexcept
{
	co_await !::bzd::typeTraits::ToStream<Args...>::process(stream, bzd::forward<Args>(args)...);
	co_return {};
}

} // namespace bzd

namespace bzd::impl {

template <class T, Size maxBufferSize>
struct ToStreamToString : ::bzd::ToString<T>
{
	template <class... Args>
	static bzd::Async<> process(bzd::OStream& stream, Args&&... args) noexcept
	{
		bzd::String<maxBufferSize> string;
		if (!::bzd::ToString<T>::process(string.assigner(), bzd::forward<Args>(args)...))
		{
			co_return bzd::error::Failure("Cannot assign value to string.");
		}
		co_await !stream.write(string.asBytes());
		co_return {};
	}
};

} // namespace bzd::impl