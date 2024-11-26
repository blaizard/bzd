#pragma once

#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/pattern/from_stream/base.hh"
#include "cc/bzd/utility/pattern/from_string/range_of_ranges.hh"

namespace bzd {

template <concepts::sameTemplate<ToRangeOfRanges> Output>
struct FromStream<Output> : public bzd::FromString<Output>
{
	template <class T>
	using Comparison = typename bzd::FromString<Output>::template Comparison<T>;

	template <concepts::generatorInputByteCopyableRange Generator, class T>
	static bzd::Async<Size> process(Generator&& generator, ToRangeOfRanges<T>& sortedRange) noexcept
	{
		Comparison<T> comparison{};

		auto first = sortedRange.input.begin();
		auto last = sortedRange.input.end();

		co_await !impl::fromStreamforEach(generator, [&](const auto c) -> Bool {
			auto [it, lastUpdated] = bzd::algorithm::equalRange(first, last, static_cast<bzd::Byte>(c), comparison);
			comparison.incrementIndex();
			first = it;
			last = lastUpdated;

			// No match, stop searching.
			if (it == lastUpdated)
			{
				return false;
			}

			// Check if there is a match and store it but continue the search
			// to match longer string if any.
			while (it != lastUpdated)
			{
				if (bzd::size(*it) == comparison.size())
				{
					sortedRange.output = it;
					break;
				}
				++it;
			}
			return true;
		});

		// Check if there was a match.
		if (sortedRange.output.hasValue())
		{
			co_return bzd::size(*(sortedRange.output.value()));
		}
		co_return bzd::error::Failure{"No match"};
	}
};

} // namespace bzd
