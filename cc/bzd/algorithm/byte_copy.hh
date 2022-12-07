#pragma once

#include "cc/bzd/container/range/subrange.hh"
#include "cc/bzd/type_traits/is_trivially_copyable.hh"
#include "cc/bzd/type_traits/iterator.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/type_traits/sentinel_for.hh"
#include "cc/bzd/utility/min.hh"

namespace bzd::algorithm {

/// Copy an input range into an output range at byte level.
///
/// \param[in] input The range of elements to copy from.
/// \param[out] output The range of the destination range.
///
/// \return The remain of the output range.
template <concepts::forwardRange InputRange, concepts::outputRange OutputRange>
requires(concepts::byteCopyableRange<InputRange> && concepts::byteCopyableRange<OutputRange>)
constexpr auto byteCopy(InputRange&& input, OutputRange&& output)
{
	auto inputFirst = bzd::begin(input);
	const auto inputLast = bzd::end(input);
	auto outputFirst = bzd::begin(output);
	const auto outputLast = bzd::end(output);

	if constexpr (concepts::sizedRange<InputRange> && concepts::sizedRange<OutputRange>)
	{
		auto n = bzd::min(bzd::size(input), bzd::size(output));
		for (; n > 0; --n)
		{
			reinterpret_cast<bzd::Byte&>(*outputFirst) = reinterpret_cast<const bzd::Byte>(*inputFirst);
			++outputFirst;
			++inputFirst;
		}
	}
	else
	{
		while (inputFirst != inputLast && outputFirst != outputLast)
		{
			reinterpret_cast<bzd::Byte&>(*outputFirst) = reinterpret_cast<const bzd::Byte>(*inputFirst);
			++outputFirst;
			++inputFirst;
		}
	}

	return range::SubRange(outputFirst, outputLast);
}

} // namespace bzd::algorithm
