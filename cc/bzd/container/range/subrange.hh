#pragma once

#include "cc/bzd/container/range/view_interface.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/type_traits/sentinel_for.hh"

namespace bzd::range {

/// Create a sub range from a pair of iterator and sentinel.
template <concepts::inputOrOutputIterator Iterator, concepts::sentinelFor<Iterator> Sentinel>
class SubRange : public ViewInterface<SubRange<Iterator, Sentinel>>
{
public:
	constexpr SubRange(Iterator begin, Sentinel end) noexcept : begin_{begin}, end_{end} {}

public:
	constexpr Iterator begin() const noexcept { return begin_; }
	constexpr Sentinel end() const noexcept { return end_; }

private:
	Iterator begin_;
	Sentinel end_;
};

} // namespace bzd::range

namespace bzd::typeTraits {
template <class Iterator, class Sentinel>
inline constexpr bzd::Bool enableBorrowedRange<bzd::range::SubRange<Iterator, Sentinel>> = true;
}
