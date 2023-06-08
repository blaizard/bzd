#pragma once

#include "cc/bzd/container/ranges/view_interface.hh"
#include "cc/bzd/container/wrapper.hh"
#include "cc/bzd/container/ranges/views/adaptor.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/advance.hh"

namespace bzd::ranges {

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
		auto begin = bzd::begin(range_.get());
		bzd::advance(begin, count_, end());
		return begin;
	}
	constexpr auto end() const noexcept { return bzd::end(range_.get()); }

private:
	bzd::Wrapper<Range> range_;
	DifferenceType count_;
};

template <class Range>
Drop(bzd::InPlace, Range&&, const auto) -> Drop<Range&&>;

inline constexpr Adaptor<Drop> drop;

} // namespace bzd::ranges

namespace bzd::typeTraits {
template <class Range>
inline constexpr bzd::Bool enableBorrowedRange<bzd::ranges::Drop<Range>> = concepts::borrowedRange<Range>;
}
