#pragma once

#include "cc/bzd/container/iterator/reverse.hh"
#include "cc/bzd/container/range/view_interface.hh"
#include "cc/bzd/container/range/views/adaptor.hh"
#include "cc/bzd/type_traits/is_same_class.hh"
#include "cc/bzd/type_traits/range.hh"

namespace bzd::range {

template <concepts::bidirectionalRange V>
requires concepts::borrowedRange<V>
class Reverse : public ViewInterface<Reverse<V>>
{
private: // Traits.
	using Iterator = typeTraits::RangeIterator<V>;
	using Sentinel = typeTraits::RangeSentinel<V>;

public:
	constexpr explicit Reverse(bzd::InPlace, auto&& view) noexcept : begin_{bzd::begin(view)}, end_{bzd::end(view)} {}

public:
	constexpr auto begin() const noexcept { return bzd::iterator::Reverse{end_}; }
	constexpr auto end() const noexcept { return bzd::iterator::Reverse{begin_}; }

private:
	Iterator begin_;
	Sentinel end_;
};

template <class T>
Reverse(bzd::InPlace, T&&) -> Reverse<T&&>;

inline constexpr Adaptor<Reverse> reverse;

} // namespace bzd::range

namespace bzd::typeTraits {
template <class V>
inline constexpr bzd::Bool enableBorrowedRange<bzd::range::Reverse<V>> = concepts::borrowedRange<V>;
}
