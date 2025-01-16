#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/bzd/utility/iterators/container_of_iterables.hh"

#include <iostream>

namespace bzd::iterator {

template <class Iterator, concepts::sentinelFor<Iterator> Sentinel>
bzd::Async<> ContainerOfIterables<Iterator, Sentinel>::operator++() noexcept
requires(concepts::asyncIterator<Iterator>)
{
	do
	{
		if constexpr (concepts::asyncIterator<NestedIterator>)
		{
			if (nested_)
			{
				co_await !(++(nested_.valueMutable()));
			}
			else
			{
				nested_ = co_await !bzd::begin(*it_);
				::std::cout << "ContainerOfIterables" << ::std::endl;
			}
		}
		else
		{
			if (nested_)
			{
				++(nested_.valueMutable());
			}
			else
			{
				nested_ = bzd::begin(*it_);
			}
		}

		if (nested_.value() != bzd::end(*it_))
		{
			break;
		}
		co_await !(++it_);
		nested_.reset();
	} while (it_ != end_);

	co_return {};
}

} // namespace bzd::iterator
