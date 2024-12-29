#pragma once

#include "cc/bzd/container/wrapper.hh"
#include "cc/bzd/type_traits/is_same_class.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/iterators/reverse.hh"
#include "cc/bzd/utility/ranges/view_interface.hh"
#include "cc/bzd/utility/ranges/views/adaptor.hh"
#include "cc/bzd/utility/ranges/views/all.hh"

namespace bzd::ranges {

template <concepts::view Range>
requires(concepts::bidirectionalRange<Range>)
class Reverse : public ViewInterface<Reverse<Range>>
{
public:
	constexpr explicit Reverse(bzd::InPlace, auto&& range) noexcept : range_{bzd::move(range)} {}

public:
	constexpr auto begin() noexcept { return bzd::iterator::Reverse{bzd::inPlace, bzd::end(range_)}; }
	constexpr auto begin() const noexcept { return bzd::iterator::Reverse{bzd::inPlace, bzd::end(range_)}; }
	constexpr auto end() noexcept { return bzd::iterator::Reverse{bzd::inPlace, bzd::begin(range_)}; }
	constexpr auto end() const noexcept { return bzd::iterator::Reverse{bzd::inPlace, bzd::begin(range_)}; }

private:
	Range range_;
};

template <class Range>
Reverse(bzd::InPlace, Range&&) -> Reverse<All<Range>>;

inline constexpr Adaptor<Reverse> reverse;

} // namespace bzd::ranges

namespace bzd::typeTraits {
template <class Range>
inline constexpr bzd::Bool enableBorrowedRange<bzd::ranges::Reverse<Range>> = concepts::borrowedRange<Range>;
}
