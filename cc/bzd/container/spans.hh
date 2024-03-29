#pragma once

#include "cc/bzd/container/array.hh"
#include "cc/bzd/container/span.hh"
#include "cc/bzd/utility/apply.hh"
#include "cc/bzd/utility/in_place.hh"
#include "cc/bzd/utility/iterators/container_of_iterables.hh"
#include "cc/bzd/utility/min.hh"

namespace bzd {

/// Spans is a container for span(s), representing data from multiple contiguous memory
/// sections.
/// The knowledge of number of spans is defined at compile time, from which iterators and channels
/// can take advantage for copying or moving data.
template <class T, Size N>
class Spans
{
public: // Traits.
	static constexpr Size nbSpans = N;
	using Self = Spans<T, N>;
	using Iterator = bzd::iterator::ContainerOfIterables<typename Array<Span<T>, N>::Iterator>;
	using ConstIterator = bzd::iterator::ContainerOfIterables<typename Array<Span<T>, N>::ConstIterator>;
	using Sentinel = bzd::iterator::Sentinel<Iterator>;
	using ConstSentinel = bzd::iterator::Sentinel<ConstIterator>;

public: // Constructors/assignments.
	constexpr Spans() noexcept = default;
	constexpr Spans(const Self&) noexcept = default;
	constexpr Self& operator=(const Self&) noexcept = default;
	constexpr Spans(Self&&) noexcept = default;
	constexpr Self& operator=(Self&&) noexcept = default;

	template <class... Args>
	constexpr Spans(InPlace, Args&&... args) noexcept : spans_{inPlace, bzd::forward<Args>(args)...}
	{
	}

public:
	/// Get the underlying container.
	[[nodiscard]] constexpr auto& spans() const noexcept { return spans_; }

	/// Get the size of the memory sequence.
	/// This is adding all span sizes and returning the sum.
	///
	/// \return The sum of all span sizes.
	[[nodiscard]] constexpr Size size() const noexcept
	{
		return bzd::apply([](const auto&... span) { return (span.size() + ...); }, spans_);
	}
	[[nodiscard]] constexpr Bool empty() const noexcept { return (size() == 0); }

	[[nodiscard]] constexpr auto& operator[](const Size index) noexcept { return spans_[index]; }
	[[nodiscard]] constexpr auto& operator[](const Size index) const noexcept { return spans_[index]; }

	/// Create a sub view of this spans.
	///
	/// \param[in] offset From where to start to copy.
	/// \param[in] count The number of elements to copy.
	/// \return A sub view of the current object.
	constexpr auto subSpans(const Size offset, const Size count = npos) const noexcept
	{
		Spans<T, N> sub{};

		for (Size index = 0, currentCount = count, currentOffset = offset; index < spans_.size() && currentCount; ++index)
		{
			const auto& span = spans_[index];
			// Ignore spans that are before offset.
			if (currentOffset > span.size())
			{
				currentOffset -= span.size();
			}
			// This is where offset is within the current span and not null.
			else if (currentOffset)
			{
				const auto spanCount = bzd::min(currentCount, span.size() - currentOffset);
				sub.spans_[index] = span.subSpan(currentOffset, spanCount);
				currentOffset = 0;
				currentCount -= (currentCount == npos) ? 0 : spanCount;
			}
			//  This is where content with no offset is added.
			else
			{
				const auto spanCount = bzd::min(currentCount, span.size());
				sub.spans_[index] = span.subSpan(0, spanCount);
				currentCount -= (currentCount == npos) ? 0 : spanCount;
			}
		}

		return sub;
	}

	constexpr auto first(const Size count) const noexcept { return subSpans(0, count); }

	constexpr auto first(const Size count) noexcept { return subSpans(0, count); }

	constexpr auto last(const Size count) const noexcept { return subSpans(size() - count, count); }

	constexpr auto last(const Size count) noexcept { return subSpans(size() - count, count); }

public: // Iterators
	[[nodiscard]] constexpr auto begin() noexcept
	{
		Iterator it{spans_.begin(), spans_.end()};
		++it;
		return it;
	}
	[[nodiscard]] constexpr auto begin() const noexcept
	{
		ConstIterator it{spans_.begin(), spans_.end()};
		++it;
		return it;
	}
	[[nodiscard]] constexpr auto end() noexcept { return Sentinel{}; }
	[[nodiscard]] constexpr auto end() const noexcept { return ConstSentinel{}; }

private:
	template <class U, Size M>
	friend class Spans;

	Array<Span<T>, N> spans_{};
};

} // namespace bzd
