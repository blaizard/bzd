#pragma once

#include "cc/bzd/container/range/view_interface.hh"
#include "cc/bzd/container/range/views/adaptor.hh"
#include "cc/bzd/type_traits/range.hh"

namespace bzd::range {

template <concepts::range V>
class All : public ViewInterface<All<V>>
{
private: // Traits.
	using Iterator = typeTraits::RangeIterator<V>;
	using Sentinel = typeTraits::RangeSentinel<V>;

public:
	/// Note the reference is important here, otherwise the range is copied.
	/// This can therefore works for every type of ranges: owning or borrowing.
	constexpr All(V& view) noexcept : begin_{bzd::begin(view)}, end_{bzd::end(view)} {}

public:
	constexpr auto begin() const noexcept { return begin_; }
	constexpr auto end() const noexcept { return end_; }

private:
	Iterator begin_;
	Sentinel end_;
};

inline constexpr Adaptor<All> all;

} // namespace bzd::range
