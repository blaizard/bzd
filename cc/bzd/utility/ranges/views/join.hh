#pragma once

#include "cc/bzd/container/wrapper.hh"
#include "cc/bzd/type_traits/is_same_class.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/apply.hh"
#include "cc/bzd/utility/iterators/container_of_iterables.hh"
#include "cc/bzd/utility/ranges/view_interface.hh"
#include "cc/bzd/utility/ranges/views/adaptor.hh"

namespace bzd::ranges {

template <concepts::borrowedRange Range>
class Join : public ViewInterface<Join<Range>>
{
private:
	using Iterator = bzd::iterator::ContainerOfIterables<bzd::typeTraits::RangeIterator<Range>, bzd::typeTraits::RangeSentinel<Range>>;

public:
	constexpr explicit Join(bzd::InPlace, auto&& range) noexcept : range_{bzd::forward<decltype(range)>(range)} {}

public:
	constexpr auto begin() const noexcept
	requires(concepts::syncRange<Range>)
	{
		Iterator it{bzd::begin(range_.get()), bzd::end(range_.get())};
		++it;
		return it;
	}

	constexpr auto end() const noexcept { return bzd::iterator::Sentinel<Iterator>{}; }

	bzd::Async<Iterator> begin() const noexcept
	requires(concepts::asyncRange<Range>);

	[[nodiscard]] constexpr Size size() const noexcept
	requires(concepts::sizedRange<Range>)
	{
		bzd::Size size{0u};
		for (const auto& range : range_.get())
		{
			size += range.size();
		}
		return size;
	}

	[[nodiscard]] constexpr Size size() const noexcept
	requires(concepts::staticSizedRange<Range>)
	{
		return bzd::apply([](const auto&... range) { return (range.size() + ...); }, range_.get());
	}

private:
	bzd::Wrapper<Range> range_;
};

template <class Range>
Join(bzd::InPlace, Range&&) -> Join<Range&&>;

inline constexpr Adaptor<Join> join;

} // namespace bzd::ranges

namespace bzd::typeTraits {
template <class Range>
inline constexpr bzd::Bool enableBorrowedRange<bzd::ranges::Join<Range>> = concepts::borrowedRange<Range>;
}
