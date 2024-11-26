#pragma once

#include "cc/bzd/algorithm/equal_range.hh"
#include "cc/bzd/container/map.hh"
#include "cc/bzd/type_traits/is_same_template.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/pattern/from_string/base.hh"

#include <iostream>

namespace bzd {

namespace concepts {
template <class T>
concept rangeOfRandomAccessByteCopyableRanges = bzd::concepts::randomAccessByteCopyableRange<typeTraits::RangeValue<T>>;
}

template <concepts::rangeOfRandomAccessByteCopyableRanges T>
struct ToRangeOfRanges
{
	const T& input;
	bzd::Optional<bzd::typeTraits::RangeIterator<T>> output{};
};

template <concepts::sameTemplate<ToRangeOfRanges> Output>
struct FromString<Output>
{
	template <class T>
	class Comparison
	{
	public:
		bool operator()(const typeTraits::RangeValue<T>& element, const bzd::Byte c) const noexcept
		{
			if (index_ >= element.size())
			{
				return true;
			}
			return element.at(index_) < static_cast<char>(c);
		}
		bool operator()(const bzd::Byte c, const typeTraits::RangeValue<T>& element) const noexcept
		{
			if (index_ >= element.size())
			{
				return false;
			}
			return static_cast<char>(c) < element.at(index_);
		}
		void incrementIndex() noexcept { ++index_; }
		constexpr Size size() const noexcept { return index_; }

	private:
		bzd::Size index_{0};
	};

	template <bzd::concepts::inputByteCopyableRange Range, class T>
	static constexpr Optional<Size> process(Range&& range, ToRangeOfRanges<T>& sortedRange) noexcept
	{
		Comparison<T> comparison{};

		auto first = sortedRange.input.begin();
		auto last = sortedRange.input.end();

		for (const auto c : range)
		{
			auto [it, lastUpdated] = bzd::algorithm::equalRange(first, last, static_cast<bzd::Byte>(c), comparison);
			comparison.incrementIndex();
			first = it;
			last = lastUpdated;

			// No match, stop searching.
			if (it == lastUpdated)
			{
				break;
			}

			// Check if there is a match and store it but continue the search
			// to match longer string if any.
			while (it != lastUpdated)
			{
				if (bzd::size(*it) == comparison.size())
				{
					sortedRange.output = it;
					break;
				}
				++it;
			}
		}

		// Check if there was a match.
		if (sortedRange.output.hasValue())
		{
			return bzd::size(*(sortedRange.output.value()));
		}
		return bzd::nullopt;
	}
};

} // namespace bzd
