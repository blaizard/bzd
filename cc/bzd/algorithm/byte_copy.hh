#pragma once

#include "cc/bzd/type_traits/is_trivially_copyable.hh"
#include "cc/bzd/type_traits/iterator.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/type_traits/sentinel_for.hh"
#include "cc/bzd/utility/min.hh"
#include "cc/bzd/utility/ranges/in_out_result.hh"

namespace bzd::algorithm {

/// Copy an input range into an output range at byte level.
///
/// \param[in] input The range of elements to copy from.
/// \param[out] output The range of the destination range.
///
/// \return The remain of the output range.
template <concepts::inputByteCopyableRange InputRange, concepts::outputByteCopyableRange OutputRange>
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
			*outputFirst = static_cast<const typeTraits::RangeValue<OutputRange>>(static_cast<const bzd::Byte>(*inputFirst));
			++outputFirst;
			++inputFirst;
		}
	}
	else
	{
		while (inputFirst != inputLast && outputFirst != outputLast)
		{
			*outputFirst = static_cast<const typeTraits::RangeValue<OutputRange>>(static_cast<const bzd::Byte>(*inputFirst));
			++outputFirst;
			++inputFirst;
		}
	}

	return ranges::InOutResult{inputFirst, outputFirst};
}

/// Copy an input range into an output range at byte level.
///
/// \param[in] input The range of elements to copy from.
/// \param[out] output The range of the destination range.
///
/// \return The remain of the output range.
template <concepts::inputByteCopyableRange InputRange, concepts::outputByteCopyableRange OutputRange>
constexpr bzd::Size byteCopyReturnSize(InputRange&& input, OutputRange&& output)
{
	auto inputFirst = bzd::begin(input);
	const auto inputLast = bzd::end(input);
	auto outputFirst = bzd::begin(output);
	const auto outputLast = bzd::end(output);

	if constexpr (concepts::sizedRange<InputRange> && concepts::sizedRange<OutputRange>)
	{
		const auto n = bzd::min(bzd::size(input), bzd::size(output));
		for (bzd::Size i = 0; i < n; ++i)
		{
			*outputFirst = static_cast<const typeTraits::RangeValue<OutputRange>>(static_cast<const bzd::Byte>(*inputFirst));
			++outputFirst;
			++inputFirst;
		}
		return n;
	}
	else
	{
		bzd::Size n = 0u;
		while (inputFirst != inputLast && outputFirst != outputLast)
		{
			*outputFirst = static_cast<const typeTraits::RangeValue<OutputRange>>(static_cast<const bzd::Byte>(*inputFirst));
			++outputFirst;
			++inputFirst;
			++n;
		}
		return n;
	}
}

} // namespace bzd::algorithm
