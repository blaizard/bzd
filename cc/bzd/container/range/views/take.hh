#pragma once

#include "cc/bzd/container/range/view_interface.hh"
#include "cc/bzd/container/range/view_storage.hh"
#include "cc/bzd/container/range/views/adaptor.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/advance.hh"

namespace bzd::range {

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
	ViewStorage<Range> range_;
	DifferenceType count_;
};

template <class Range>
Take(bzd::InPlace, Range&&, const auto) -> Take<Range&&>;

inline constexpr Adaptor<Take> take;

} // namespace bzd::range

namespace bzd::typeTraits {
template <class Range>
inline constexpr bzd::Bool enableBorrowedRange<bzd::range::Take<Range>> = concepts::borrowedRange<Range>;
}
