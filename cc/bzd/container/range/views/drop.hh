#pragma once

#include "cc/bzd/container/range/view_interface.hh"
#include "cc/bzd/container/range/views/adaptor.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/advance.hh"

namespace bzd::range {

template <concepts::borrowedRange Range>
class Drop : public ViewInterface<Drop<Range>>
{
private: // Traits.
	using Iterator = typeTraits::RangeIterator<Range>;
	using DifferenceType = typeTraits::IteratorDifference<Iterator>;

public:
	constexpr Drop(bzd::InPlace, auto&& range, const DifferenceType count) noexcept :
		range_{bzd::forward<decltype(range)>(range)}, count_{count}
	{
	}

public:
	constexpr auto begin() const noexcept
	{
		auto begin = bzd::begin(range_);
		bzd::advance(begin, count_, end());
		return begin;
	}
	constexpr auto end() const noexcept { return bzd::end(range_); }

private:
	Range range_;
	DifferenceType count_;
};

template <class Range>
Drop(bzd::InPlace, Range&&, const auto) -> Drop<Range&&>;

inline constexpr Adaptor<Drop> drop;

} // namespace bzd::range

namespace bzd::typeTraits {
template <class Range>
inline constexpr bzd::Bool enableBorrowedRange<bzd::range::Drop<Range>> = concepts::borrowedRange<Range>;
}
