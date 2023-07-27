#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/bzd/utility/advance.hh"

namespace bzd {

/// Increments given iterator `it` for `n` times.
template <concepts::inputOrOutputIterator Iterator>
requires(concepts::asyncIterator<Iterator>)
bzd::Async<> advance(Iterator& it, const typeTraits::IteratorDifference<Iterator> n) noexcept
{
	auto dist = n;
	if constexpr (concepts::randomAccessIterator<Iterator>)
	{
		co_await !(it += dist);
	}
	else
	{
		while (dist > 0)
		{
			--dist;
			co_await !++it;
		}

		// To support a negative `n` value.
		if constexpr (concepts::bidirectionalIterator<Iterator>)
		{
			while (dist < 0)
			{
				++dist;
				co_await !--it;
			}
		}
	}
	co_return {};
}

/// Increments given iterator `it` until `it` == `bound`.
template <concepts::inputOrOutputIterator Iterator, concepts::sentinelFor<Iterator> Sentinel>
requires(concepts::asyncIterator<Iterator>)
bzd::Async<> advance(Iterator& it, Sentinel bound) noexcept
{
	if constexpr (concepts::assignableFrom<Iterator&, Sentinel>)
	{
		it = bzd::move(bound);
	}
	else if constexpr (concepts::sizedSentinelFor<Sentinel, Iterator>)
	{
		co_await !bzd::advance(it, bound - it);
	}
	else
	{
		while (it != bound)
		{
			co_await !++it;
		}
	}
	co_return {};
}

/// Increments given iterator `it` for `n` times, or until `it` == `bound`, whichever comes first.
template <concepts::inputOrOutputIterator Iterator, concepts::sentinelFor<Iterator> Sentinel>
requires(concepts::asyncIterator<Iterator>)
bzd::Async<typeTraits::IteratorDifference<Iterator>> advance(Iterator& it,
															 const typeTraits::IteratorDifference<Iterator> n,
															 Sentinel bound) noexcept
{
	using ResultType = typeTraits::IteratorDifference<Iterator>;

	if constexpr (concepts::sizedSentinelFor<Sentinel, Iterator>)
	{
		const ResultType dist = bzd::abs(bound - it) - bzd::abs(n);
		if (dist < 0)
		{
			co_await !bzd::advance(it, bound);
			co_return -dist;
		}
		co_await !bzd::advance(it, n);
		co_return ResultType{0};
	}
	else
	{
		ResultType dist = n;

		while (dist > 0 && it != bound)
		{
			--dist;
			co_await !++it;
		}

		// To support a negative `n` value.
		if constexpr (concepts::bidirectionalIterator<Iterator>)
		{
			while (dist < 0 && it != bound)
			{
				++dist;
				co_await !--it;
			}
		}

		co_return dist;
	}
}

} // namespace bzd
