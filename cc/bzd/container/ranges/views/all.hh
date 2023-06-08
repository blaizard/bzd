#pragma once

#include "cc/bzd/container/ranges/view_interface.hh"
#include "cc/bzd/container/ranges/views/adaptor.hh"
#include "cc/bzd/container/wrapper.hh"
#include "cc/bzd/type_traits/is_same_class.hh"
#include "cc/bzd/type_traits/range.hh"

namespace bzd::ranges {

template <concepts::borrowedRange Range>
class All : public ViewInterface<All<Range>>
{
public:
	constexpr explicit All(bzd::InPlace, auto&& range) noexcept : range_{bzd::forward<decltype(range)>(range)} {}

public:
	constexpr auto begin() const noexcept { return bzd::begin(range_.get()); }
	constexpr auto end() const noexcept { return bzd::end(range_.get()); }

private:
	bzd::Wrapper<Range> range_;
};

template <class Range>
All(bzd::InPlace, Range&&) -> All<Range&&>;

inline constexpr Adaptor<All> all;

} // namespace bzd::ranges

namespace bzd::typeTraits {
template <class Range>
inline constexpr bzd::Bool enableBorrowedRange<bzd::ranges::All<Range>> = concepts::borrowedRange<Range>;
}
