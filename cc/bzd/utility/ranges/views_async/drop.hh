#pragma once

#include "cc/bzd/core/async.hh"
#include "cc/bzd/utility/iterators/advance_async.hh"
#include "cc/bzd/utility/ranges/views/drop.hh"

namespace bzd::ranges {

template <concepts::view Range>
bzd::Async<typename Drop<Range>::Iterator> Drop<Range>::begin() const noexcept
requires(concepts::asyncRange<Range>)
{
	auto begin = co_await !bzd::begin(range_);
	co_await !bzd::advance(begin, count_, end());
	co_return begin;
}

} // namespace bzd::ranges
