#pragma once

#include "cc/bzd/container/range/view_interface.hh"
#include "cc/bzd/type_traits/sentinel_for.hh"

namespace bzd::range {

/// Create a sub range from a
template <concepts::inputOrOutputIterator Iterator, concepts::sentinelFor<Iterator> Sentinel>
class SubRange : public ViewInterface<SubRange<Iterator, Sentinel>>
{
public:
	constexpr SubRange(const Iterator begin, const Sentinel end) noexcept : begin_{begin}, end_{end} {}

public:
	constexpr Iterator begin() const noexcept { return begin_; }
	constexpr Sentinel end() const noexcept { return end_; }

private:
	Iterator begin_;
	Sentinel end_;
};

} // namespace bzd::range
