#pragma once

#include "cc/bzd/algorithm/find_if.hh"
#include "cc/bzd/container/wrapper.hh"
#include "cc/bzd/type_traits/predicate.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/ranges/view_interface.hh"
#include "cc/bzd/utility/ranges/views/adaptor.hh"
#include "cc/bzd/utility/ranges/views/adaptor_iterator.hh"
#include "cc/bzd/utility/ranges/views/all.hh"

namespace bzd::ranges {

template <concepts::view Range, concepts::predicate<typeTraits::RangeValue<Range>> UnaryPredicate>
class Filter : public ViewInterface<Filter<Range, UnaryPredicate>>
{
private: // Traits.
	class Iterator : public AdapatorForwardIterator<Range, Iterator>
	{
	public:
		constexpr Iterator(auto&& it, const Filter& filter) noexcept :
			AdapatorForwardIterator<Range, Iterator>{bzd::move(it)}, filter_{filter}
		{
		}
		constexpr void next(auto& it) noexcept { it = bzd::algorithm::findIf(++it, filter_->end(), filter_->predicate_); }

	private:
		bzd::ReferenceWrapper<const Filter> filter_;
	};

public:
	constexpr Filter(bzd::InPlace, auto&& range, const UnaryPredicate& predicate) noexcept : range_{bzd::move(range)}, predicate_{predicate}
	{
	}

public:
	constexpr auto begin() const noexcept
	{
		auto it = bzd::algorithm::findIf(bzd::begin(range_), bzd::end(range_), predicate_);
		return Iterator{bzd::move(it), *this};
	}
	constexpr auto end() const noexcept { return bzd::end(range_); }

private:
	Range range_;
	UnaryPredicate predicate_;
};

template <class Range, class UnaryPredicate>
Filter(bzd::InPlace, Range&&, UnaryPredicate) -> Filter<All<Range&&>, typeTraits::RemoveReference<UnaryPredicate>>;

inline constexpr Adaptor<Filter> filter;

} // namespace bzd::ranges

namespace bzd::typeTraits {
template <class Range, class UnaryPredicate>
inline constexpr bzd::Bool enableBorrowedRange<bzd::ranges::Filter<Range, UnaryPredicate>> = concepts::borrowedRange<Range>;
}
