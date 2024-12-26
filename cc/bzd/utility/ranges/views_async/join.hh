#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/bzd/utility/iterators/container_of_iterables_async.hh"
#include "cc/bzd/utility/ranges/views/join.hh"

namespace bzd::ranges {

template <concepts::view Range>
bzd::Async<typename Join<Range>::Iterator> Join<Range>::begin() const noexcept
requires(concepts::asyncRange<Range>)
{
	auto begin = co_await !bzd::begin(range_);
	Iterator it{begin, bzd::end(range_)};
	co_await !(++it);
	co_return it;
}

} // namespace bzd::ranges
