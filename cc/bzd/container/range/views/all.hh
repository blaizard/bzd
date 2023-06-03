#pragma once

#include "cc/bzd/container/range/view_interface.hh"
#include "cc/bzd/container/range/view_storage.hh"
#include "cc/bzd/container/range/views/adaptor.hh"
#include "cc/bzd/type_traits/is_same_class.hh"
#include "cc/bzd/type_traits/range.hh"

namespace bzd::range {

template <concepts::borrowedRange Range>
class All : public ViewInterface<All<Range>>
{
public:
	constexpr explicit All(bzd::InPlace, auto&& range) noexcept : range_{bzd::forward<decltype(range)>(range)} {}

public:
	constexpr auto begin() const noexcept { return bzd::begin(range_.get()); }
	constexpr auto end() const noexcept { return bzd::end(range_.get()); }

private:
	ViewStorage<Range> range_;
};

template <class Range>
All(bzd::InPlace, Range&&) -> All<Range&&>;

inline constexpr Adaptor<All> all;

} // namespace bzd::range

namespace bzd::typeTraits {
template <class Range>
inline constexpr bzd::Bool enableBorrowedRange<bzd::range::All<Range>> = concepts::borrowedRange<Range>;
}
