#pragma once

#include "cc/bzd/algorithm/equal_range.hh"
#include "cc/bzd/container/map.hh"
#include "cc/bzd/container/result.hh"
#include "cc/bzd/container/wrapper.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/utility/pattern/from_string/base.hh"

namespace bzd {

namespace concepts {
template <class T>
concept rangeOfRandomAccessByteCopyableRanges = bzd::concepts::randomAccessByteCopyableRange<typeTraits::RangeValue<T>>;
} // namespace concepts

namespace impl::pattern {
struct AccessorNoop
{
	constexpr auto operator()(const auto& element) const noexcept { return element; }
};
struct AccessorMap
{
	constexpr auto operator()(const auto& element) const noexcept { return element.first; }
};
} // namespace impl::pattern

template <class T, class Accessor>
struct ToSortedRangeOfRanges
{
	constexpr ToSortedRangeOfRanges(auto&& input_, const Accessor accessor_ = Accessor{}) noexcept :
		input{bzd::forward<decltype(input_)>(input_)}, accessor{accessor_}
	{
	}

	/// Accessor for the constant output.
	///
	/// The output is expected to exist.
	constexpr const bzd::typeTraits::RangeIterator<T>& value() const noexcept { return output.value(); }

	/// Accessor for the mutable output.
	///
	/// The output is expected to exist.
	constexpr bzd::typeTraits::RangeIterator<T>& valueMutable() noexcept { return output.valueMutable(); }

	bzd::Wrapper<T> input;
	const Accessor accessor;
	bzd::Optional<bzd::typeTraits::RangeIterator<T>> output{};
};
template <concepts::rangeOfRandomAccessByteCopyableRanges T>
ToSortedRangeOfRanges(T&&) -> ToSortedRangeOfRanges<T, impl::pattern::AccessorNoop>;
template <concepts::map T>
ToSortedRangeOfRanges(T&&) -> ToSortedRangeOfRanges<T&, impl::pattern::AccessorMap>;

template <concepts::sameTemplate<ToSortedRangeOfRanges> Output>
struct FromString<Output>
{
protected:
	template <class T, class Accessor>
	class Comparison
	{
	public:
		using Iterator = bzd::typeTraits::RangeIterator<T>;

	public:
		constexpr Comparison(const T& range, const Accessor& accessor) noexcept :
			first_{bzd::begin(range)}, last_{bzd::end(range)}, accessor_{accessor}
		{
		}

		constexpr bool operator()(const typeTraits::RangeValue<T>& element, const bzd::Byte c) const noexcept
		{
			const auto actualElement = accessor_(element);
			if (index_ >= bzd::size(actualElement))
			{
				return true;
			}
			return actualElement.at(index_) < static_cast<char>(c);
		}

		constexpr bool operator()(const bzd::Byte c, const typeTraits::RangeValue<T>& element) const noexcept
		{
			const auto actualElement = accessor_(element);
			if (index_ >= bzd::size(actualElement))
			{
				return false;
			}
			return static_cast<char>(c) < actualElement.at(index_);
		}

		/// Process a new character.
		///
		/// \return An error result if there is no match.
		///         An empty result if there are matched but no full match.
		///         An iterator if there is a full match.
		constexpr bzd::Result<bzd::Optional<Iterator>> process(const bzd::Byte c) noexcept
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
				if (bzd::size(accessor_(*it)) == index_)
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
		const typeTraits::RemoveCVRef<Accessor> accessor_;
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
	static constexpr Optional<Size> process(Range&& range, T& sortedRange, const Metadata metadata = Metadata{}) noexcept
	{
		sortedRange.output.reset();
		auto comparison =
			Comparison<decltype(sortedRange.input.get()), decltype(sortedRange.accessor)>{sortedRange.input.get(), sortedRange.accessor};
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
			return bzd::size(sortedRange.accessor(*(sortedRange.output.value())));
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
