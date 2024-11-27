#pragma once

#include "cc/bzd/algorithm/equal_range.hh"
#include "cc/bzd/container/map.hh"
#include "cc/bzd/container/result.hh"
#include "cc/bzd/type_traits/is_same_template.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/pattern/from_string/base.hh"

namespace bzd {

namespace concepts {
template <class T>
concept rangeOfRandomAccessByteCopyableRanges = bzd::concepts::randomAccessByteCopyableRange<typeTraits::RangeValue<T>>;
}

template <concepts::rangeOfRandomAccessByteCopyableRanges T>
struct ToSortedRangeOfRanges
{
	const T& input;
	bzd::Optional<bzd::typeTraits::RangeIterator<T>> output{};
};

template <concepts::sameTemplate<ToSortedRangeOfRanges> Output>
struct FromString<Output>
{
protected:
	template <class T>
	class Comparison
	{
	public:
		using Iterator = bzd::typeTraits::RangeIterator<T>;

	public:
		constexpr Comparison(const T& range) noexcept : first_{bzd::begin(range)}, last_{bzd::end(range)} {}

		bool operator()(const typeTraits::RangeValue<T>& element, const bzd::Byte c) const noexcept
		{
			if (index_ >= bzd::size(element))
			{
				return true;
			}
			return element.at(index_) < static_cast<char>(c);
		}

		bool operator()(const bzd::Byte c, const typeTraits::RangeValue<T>& element) const noexcept
		{
			if (index_ >= bzd::size(element))
			{
				return false;
			}
			return static_cast<char>(c) < element.at(index_);
		}

		/// Process a new character.
		///
		/// \return An error result if there is no match.
		///         An empty result if there are matched but no full match.
		///         An iterator if there is a full match.
		bzd::Result<bzd::Optional<Iterator>> process(const bzd::Byte c) noexcept
		{
			auto [it, lastUpdated] = bzd::algorithm::equalRange(first_, last_, c, *this);
			++index_;
			first_ = it;
			last_ = lastUpdated;

			// No match, return an error result.
			if (it == lastUpdated)
			{
				return bzd::error::make();
			}

			// Check if there is a match and return it.
			while (it != lastUpdated)
			{
				if (bzd::size(*it) == index_)
				{
					return it;
				}
				++it;
			}

			return bzd::nullopt;
		}

	private:
		Iterator first_;
		Iterator last_;
		bzd::Size index_{0};
	};

public:
	struct Metadata
	{
		enum class Mode
		{
			lazy,
			greedy,
		};
		// By default it uses a greedy algorithm to match the range.
		Mode mode{Mode::greedy};
	};

	template <bzd::concepts::inputByteCopyableRange Range, class T>
	static constexpr Optional<Size> process(Range&& range,
											ToSortedRangeOfRanges<T>& sortedRange,
											const Metadata metadata = Metadata{}) noexcept
	{
		Comparison<T> comparison{sortedRange.input};
		for (const auto c : range)
		{
			const auto maybeResult = comparison.process(static_cast<bzd::Byte>(c));

			if (maybeResult.hasError())
			{
				break;
			}

			if (maybeResult.value().hasValue())
			{
				sortedRange.output = maybeResult.value().value();
			}

			// Dot not look further if lazy mode.
			if (metadata.mode == Metadata::Mode::lazy && sortedRange.output.hasValue())
			{
				break;
			}
		}

		// Check if there was a match.
		if (sortedRange.output.hasValue())
		{
			return bzd::size(*(sortedRange.output.value()));
		}
		return bzd::nullopt;
	}

	template <class Adapter>
	static constexpr Metadata parse(bzd::StringView options) noexcept
	{
		Metadata metadata{};

		while (!options.empty())
		{
			switch (options.front())
			{
			// Mode
			case '?':
				metadata.mode = Metadata::Mode::lazy;
				break;
			default:
				Adapter::onError("Unsupported option, only the following is supported: [?]");
			}
			options.removePrefix(1);
		}

		return metadata;
	}
};

} // namespace bzd
