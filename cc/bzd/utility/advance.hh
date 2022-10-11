#pragma once

#include "cc/bzd/math/abs.hh"
#include "cc/bzd/platform/types.hh"
#include "cc/bzd/type_traits/assignable_from.hh"
#include "cc/bzd/type_traits/iterator.hh"
#include "cc/bzd/type_traits/sentinel_for.hh"
#include "cc/bzd/type_traits/sized_sentinel_for.hh"
#include "cc/bzd/utility/move.hh"

namespace bzd {

/// Increments given iterator `it` for `n` times.
template <concepts::inputOrOutputIterator Iterator>
constexpr void advance(Iterator& it, const typeTraits::IteratorDifference<Iterator> n) noexcept
{
	auto dist = n;
	if constexpr (concepts::randomAccessIterator<Iterator>)
	{
		it += dist;
	}
	else
	{
		while (dist > 0)
		{
			--dist;
			++it;
		}

		// To support a negative `n` value.
		if constexpr (concepts::bidirectionalIterator<Iterator>)
		{
			while (dist < 0)
			{
				++dist;
				--it;
			}
		}
	}
}

/// Increments given iterator `it` until `it` == `bound`.
template <concepts::inputOrOutputIterator Iterator, concepts::sentinelFor<Iterator> Sentinel>
constexpr void advance(Iterator& it, Sentinel bound) noexcept
{
	if constexpr (concepts::assignableFrom<Iterator&, Sentinel>)
	{
		it = bzd::move(bound);
	}
	else if constexpr (concepts::sizedSentinelFor<Sentinel, Iterator>)
	{
		bzd::advance(it, bound - it);
	}
	else
	{
		while (it != bound)
		{
			++it;
		}
	}
}

/// Increments given iterator `it` for `n` times, or until `it` == `bound`, whichever comes first.
template <concepts::inputOrOutputIterator Iterator, concepts::sentinelFor<Iterator> Sentinel>
constexpr auto advance(Iterator& it, const typeTraits::IteratorDifference<Iterator> n, Sentinel bound) noexcept
{
	using ResultType = typeTraits::IteratorDifference<Iterator>;

	if constexpr (concepts::sizedSentinelFor<Sentinel, Iterator>)
	{
		const ResultType dist = bzd::abs(bound - it) - bzd::abs(n);
		if (dist < 0)
		{
			bzd::advance(it, bound);
			return -dist;
		}
		bzd::advance(it, n);
		return ResultType{0};
	}
	else
	{
		ResultType dist = n;

		while (dist > 0 && it != bound)
		{
			--dist;
			++it;
		}

		// To support a negative `n` value.
		if constexpr (concepts::bidirectionalIterator<Iterator>)
		{
			while (dist < 0 && it != bound)
			{
				++dist;
				--it;
			}
		}

		return dist;
	}
}

} // namespace bzd
