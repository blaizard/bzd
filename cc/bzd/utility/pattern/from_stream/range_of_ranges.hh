#pragma once

#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/pattern/from_stream/base.hh"
#include "cc/bzd/utility/pattern/from_string/range_of_ranges.hh"

namespace bzd {

template <concepts::sameTemplate<ToSortedRangeOfRanges> Output>
struct FromStream<Output> : public bzd::FromString<Output>
{
	using Metadata = typename bzd::FromString<Output>::Metadata;
	template <class T, class Accessor>
	using Comparison = bzd::FromString<Output>::template Comparison<T, Accessor>;

	template <concepts::asyncInputByteCopyableRange Generator, class T>
	static bzd::Async<Size> process(Generator&& generator, T& sortedRange, const Metadata metadata = Metadata{}) noexcept
	{
		sortedRange.output.reset();
		auto comparison =
			Comparison<decltype(sortedRange.input.get()), decltype(sortedRange.accessor)>{sortedRange.input.get(), sortedRange.accessor};

		co_await !bzd::async::forEach(generator, [&](const auto c) -> Bool {
			const auto maybeResult = comparison.process(static_cast<bzd::Byte>(c));

			if (maybeResult.hasError())
			{
				return false;
			}

			if (maybeResult.value().hasValue())
			{
				sortedRange.output = maybeResult.value().value();
			}

			// Dot not look further if lazy mode.
			if (metadata.mode == Metadata::Mode::lazy && sortedRange.output.hasValue())
			{
				return false;
			}

			return true;
		});

		// Check if there was a match.
		if (sortedRange.output.hasValue())
		{
			co_return bzd::size(sortedRange.accessor(*(sortedRange.output.value())));
		}
		co_return bzd::error::Failure{"No match"};
	}
};

} // namespace bzd
