#pragma once

#include "cc/bzd/container/ranges/view_interface.hh"
#include "cc/bzd/container/ranges/views/adaptor.hh"
#include "cc/bzd/container/wrapper.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/advance.hh"

namespace bzd::ranges {

template <concepts::borrowedRange Range>
class Take : public ViewInterface<Take<Range>>
{
private: // Traits.
	using Iterator = typeTraits::RangeIterator<Range>;
	using DifferenceType = typeTraits::IteratorDifference<Iterator>;

public:
	constexpr Take(bzd::InPlace, auto&& range, const DifferenceType count) noexcept :
		range_{bzd::forward<decltype(range)>(range)}, count_{count}
	{
	}

public:
	constexpr auto begin() const noexcept { return bzd::begin(range_.get()); }
	constexpr auto end() const noexcept
	{
		auto end = begin();
		bzd::advance(end, count_, bzd::end(range_.get()));
		return end;
	}

private:
	bzd::Wrapper<Range> range_;
	DifferenceType count_;
};

template <class Range>
Take(bzd::InPlace, Range&&, const auto) -> Take<Range&&>;

inline constexpr Adaptor<Take> take;

} // namespace bzd::ranges

namespace bzd::typeTraits {
template <class Range>
inline constexpr bzd::Bool enableBorrowedRange<bzd::ranges::Take<Range>> = concepts::borrowedRange<Range>;
}
