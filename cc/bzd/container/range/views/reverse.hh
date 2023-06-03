#pragma once

#include "cc/bzd/container/iterator/reverse.hh"
#include "cc/bzd/container/range/view_interface.hh"
#include "cc/bzd/container/range/views/adaptor.hh"
#include "cc/bzd/type_traits/is_same_class.hh"
#include "cc/bzd/type_traits/range.hh"

namespace bzd::range {

template <concepts::bidirectionalRange Range>
requires concepts::borrowedRange<Range>
class Reverse : public ViewInterface<Reverse<Range>>
{
public:
	constexpr explicit Reverse(bzd::InPlace, auto&& range) noexcept : range_{bzd::forward<decltype(range)>(range)} {}

public:
	constexpr auto begin() const noexcept { return bzd::iterator::Reverse{bzd::end(range_)}; }
	constexpr auto end() const noexcept { return bzd::iterator::Reverse{bzd::begin(range_)}; }

private:
	Range range_;
};

template <class Range>
Reverse(bzd::InPlace, Range&&) -> Reverse<Range&&>;

inline constexpr Adaptor<Reverse> reverse;

} // namespace bzd::range

namespace bzd::typeTraits {
template <class Range>
inline constexpr bzd::Bool enableBorrowedRange<bzd::range::Reverse<Range>> = concepts::borrowedRange<Range>;
}
