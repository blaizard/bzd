#pragma once

#include "cc/bzd/container/wrapper.hh"
#include "cc/bzd/core/async/forward.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/iterators/advance.hh"
#include "cc/bzd/utility/ranges/view_interface.hh"
#include "cc/bzd/utility/ranges/views/adaptor.hh"
#include "cc/bzd/utility/ranges/views/all.hh"

namespace bzd::ranges {

template <concepts::view Range>
requires(concepts::bidirectionalRange<Range>)
class DropLast : public ViewInterface<DropLast<Range>>
{
private: // Traits.
	using Iterator = typeTraits::RangeIterator<Range>;
	using DifferenceType = typeTraits::IteratorDifference<Iterator>;

public:
	constexpr DropLast(bzd::InPlace, auto&& range, const DifferenceType count) noexcept : range_{bzd::move(range)}, count_{count} {}

public:
	constexpr auto begin() const noexcept { return bzd::begin(range_); }

	constexpr auto end() const noexcept
	{
		auto end = bzd::end(range_);
		bzd::advance(end, -count_, begin());
		return end;
	}

private:
	Range range_;
	DifferenceType count_;
};

template <class Range>
DropLast(bzd::InPlace, Range&&, const auto) -> DropLast<All<Range>>;

inline constexpr Adaptor<DropLast> dropLast;

} // namespace bzd::ranges

namespace bzd::typeTraits {
template <class Range>
inline constexpr bzd::Bool enableBorrowedRange<bzd::ranges::DropLast<Range>> = concepts::borrowedRange<Range>;
}
