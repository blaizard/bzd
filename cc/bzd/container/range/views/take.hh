#pragma once

#include "cc/bzd/container/range/view_interface.hh"
#include "cc/bzd/container/range/views/adaptor.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/advance.hh"

namespace bzd::range {

template <concepts::range V>
class Take : public ViewInterface<Take<V>>
{
private: // Traits.
	using Iterator = typeTraits::RangeIterator<V>;
	using Sentinel = typeTraits::RangeIterator<V>;
	using DifferenceType = typeTraits::IteratorDifference<Iterator>;

public:
	/// Note the reference is important here, otherwise the range is copied.
	/// This can therefore works for every type of ranges: owning or borrowing.
	constexpr Take(V& view, const DifferenceType count) noexcept : begin_{bzd::begin(view)}, end_{bzd::begin(view)}
	{
		bzd::advance(end_, count, bzd::end(view));
	}

public:
	constexpr Iterator begin() const noexcept { return begin_; }
	constexpr Sentinel end() const noexcept { return end_; }

private:
	Iterator begin_;
	Sentinel end_;
};

inline constexpr Adaptor<Take> take;

} // namespace bzd::range
