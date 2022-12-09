#pragma once

#include "cc/bzd/container/range/in_out_result.hh"
#include "cc/bzd/type_traits/iterator.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/type_traits/sentinel_for.hh"
#include "cc/bzd/utility/min.hh"

namespace bzd::algorithm {

/// Copies all elements in the range [first, last) starting from first and proceeding to last - 1.
///
/// \param[in] first The beginning of the range of elements to copy from.
/// \param[in] last The ending of the range of elements to copy from.
/// \param[out] result The beginning of the destination range.
///
/// \return Output iterator to the element in the destination range, one past the last element copied.
template <concepts::inputIterator InputIt, concepts::sentinelFor<InputIt> InputSentinel, concepts::outputIterator OutputIt>
constexpr OutputIt copy(InputIt first, const InputSentinel last, OutputIt result) noexcept
{
	while (first != last)
	{
		*result = *first;
		++result;
		++first;
	}
	return result;
}

/// \copydoc copy
/// \param[in] input The range of elements to copy from.
/// \param[out] output The beginning of the destination range.
template <concepts::inputRange InputRange, concepts::outputIterator Output>
constexpr auto copy(InputRange&& input, Output output)
{
	return bzd::algorithm::copy(bzd::begin(input), bzd::end(input), output);
}

/// \copydoc copy
/// \param[in] input The range of elements to copy from.
/// \param[out] output The range of the destination range.
///
/// \return The remain of the output range.
template <concepts::inputRange InputRange, concepts::outputRange OutputRange>
constexpr auto copy(InputRange&& input, OutputRange&& output)
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
			*outputFirst = *inputFirst;
			++outputFirst;
			++inputFirst;
		}
	}
	else
	{
		while (inputFirst != inputLast && outputFirst != outputLast)
		{
			*outputFirst = *inputFirst;
			++outputFirst;
			++inputFirst;
		}
	}

	return range::InOutResult{inputFirst, outputFirst};
}

} // namespace bzd::algorithm
