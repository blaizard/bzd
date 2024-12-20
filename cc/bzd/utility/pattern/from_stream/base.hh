#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/bzd/meta/always_false.hh"
#include "cc/bzd/utility/ranges/stream.hh"

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
/// \param generator The input to be read from.
/// \param args The value(s) to be read.
/// \return The number of bytes read in case of success, otherwise an empty result.
template <concepts::generatorInputByteCopyableRange Generator, class... Args>
bzd::Async<Size> fromStream(Generator&& generator, Args&&... args) noexcept
{
	co_return co_await ::bzd::typeTraits::FromStream<Args...>::process(bzd::forward<Generator>(generator), bzd::forward<Args>(args)...);
}

} // namespace bzd

namespace bzd::impl {

template <concepts::generatorInputByteCopyableRange Generator, class Callback>
bzd::Async<Size> fromStreamforEach(Generator&& generator, Callback&& callback) noexcept
{
	Size count{0u};
	co_await !bzd::async::forEach(bzd::forward<Generator>(generator), [&](auto& range) -> bool {
		auto it = bzd::begin(range);
		auto end = bzd::end(range);
		if (it == end)
		{
			return false;
		}
		while (it != end)
		{
			if (!callback(*it))
			{
				return false;
			}
			++count;
			++it;
		}
		return true;
	});
	co_return count;
}

} // namespace bzd::impl
