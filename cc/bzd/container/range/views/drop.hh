#pragma once

#include "cc/bzd/container/range/view_interface.hh"
#include "cc/bzd/container/range/views/adaptor.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/advance.hh"

namespace bzd::range {

template <concepts::range V>
class Drop : public ViewInterface<Drop<V>>
{
public: // Traits.
	using Iterator = typeTraits::RangeIterator<V>;
	using Sentinel = typeTraits::RangeSentinel<V>;
	using DifferenceType = typeTraits::IteratorDifference<Iterator>;

public:
	/// Note the reference is important here, otherwise the range is copied.
	/// This can therefore works for every type of ranges: owning or borrowing.
	constexpr Drop(V& view, const DifferenceType count) noexcept : begin_{bzd::begin(view)}, end_{bzd::end(view)}
	{
		bzd::advance(begin_, count, end_);
	}

public:
	constexpr Iterator begin() const noexcept { return begin_; }
	constexpr Sentinel end() const noexcept { return end_; }

private:
	Iterator begin_;
	Sentinel end_;
};

inline constexpr Adaptor<Drop> drop;

} // namespace bzd::range
