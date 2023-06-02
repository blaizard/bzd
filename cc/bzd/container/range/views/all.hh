#pragma once

#include "cc/bzd/container/range/view_interface.hh"
#include "cc/bzd/container/range/views/adaptor.hh"
#include "cc/bzd/type_traits/is_same_class.hh"
#include "cc/bzd/type_traits/range.hh"

namespace bzd::range {

template <concepts::borrowedRange V>
class All : public ViewInterface<All<V>>
{
private: // Traits.
	using Iterator = typeTraits::RangeIterator<V>;
	using Sentinel = typeTraits::RangeSentinel<V>;

public:
	constexpr explicit All(bzd::InPlace, auto&& view) noexcept : begin_{bzd::begin(view)}, end_{bzd::end(view)} {}

public:
	constexpr auto begin() const noexcept { return begin_; }
	constexpr auto end() const noexcept { return end_; }

private:
	Iterator begin_;
	Sentinel end_;
};

template <class T>
All(bzd::InPlace, T&&) -> All<T&&>;

inline constexpr Adaptor<All> all;

} // namespace bzd::range

namespace bzd::typeTraits {
template <class V>
inline constexpr bzd::Bool enableBorrowedRange<bzd::range::All<V>> = concepts::borrowedRange<V>;
}
