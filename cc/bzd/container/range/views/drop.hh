#pragma once

#include "cc/bzd/container/range/view_interface.hh"
#include "cc/bzd/utility/advance.hh"

namespace bzd::range {

template <concepts::range V>
class Drop : public ViewInterface<Drop<V>>
{
public:
	using Iterator = typename typeTraits::Range<V>::Iterator;
	using Sentinel = typename typeTraits::Range<V>::Sentinel;
	using DifferenceType = typename typeTraits::Iterator<Iterator>::DifferenceType;

public:
	/// Note the reference is important here, otherwise the range is copied.
	/// This can therefore works for every type of ranges: owning or borrowing.
	constexpr Drop(V& view, const DifferenceType count) noexcept : begin_{bzd::begin(view)}, end_{bzd::end(view)}
	{
		bzd::advance(begin_, count, end_);
	}

	constexpr Iterator begin() const noexcept { return begin_; }

	constexpr Sentinel end() const noexcept { return end_; }

private:
	Iterator begin_;
	Sentinel end_;
};

} // namespace bzd::range
