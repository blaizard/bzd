#pragma once

#include "cc/bzd/container/range/view_interface.hh"
#include "cc/bzd/container/range/views/adaptor.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/advance.hh"

namespace bzd::range {

template <concepts::borrowedRange V>
class Drop : public ViewInterface<Drop<V>>
{
private: // Traits.
	using Iterator = typeTraits::RangeIterator<V>;
	using Sentinel = typeTraits::RangeSentinel<V>;
	using DifferenceType = typeTraits::IteratorDifference<Iterator>;

public:
	constexpr Drop(bzd::InPlace, auto&& view, const DifferenceType count) noexcept : begin_{bzd::begin(view)}, end_{bzd::end(view)}
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

template <class T>
Drop(bzd::InPlace, T&&, const auto) -> Drop<T&&>;

inline constexpr Adaptor<Drop> drop;

} // namespace bzd::range

namespace bzd::typeTraits {
template <class V>
inline constexpr bzd::Bool enableBorrowedRange<bzd::range::Drop<V>> = concepts::borrowedRange<V>;
}
