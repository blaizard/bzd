#pragma once

#include "cc/bzd/container/ranges/views/drop.hh"
#include "cc/bzd/core/async.hh"
#include "cc/bzd/utility/advance_async.hh"

namespace bzd::ranges {

template <concepts::borrowedRange Range>
bzd::Async<typename Drop<Range>::Iterator> Drop<Range>::begin() const noexcept
requires(concepts::asyncRange<Range>)
{
	auto begin = co_await !bzd::begin(range_.get());
	co_await !bzd::advance(begin, count_, end());
	co_return begin;
}

} // namespace bzd::ranges
