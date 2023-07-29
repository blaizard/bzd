#pragma once

#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/type_traits/sentinel_for.hh"
#include "cc/bzd/utility/ranges/view_interface.hh"

namespace bzd::ranges {

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

} // namespace bzd::ranges

namespace bzd::typeTraits {
template <class Iterator, class Sentinel>
inline constexpr bzd::Bool enableBorrowedRange<bzd::ranges::SubRange<Iterator, Sentinel>> = true;
}
