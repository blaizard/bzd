#pragma once

#include "cc/bzd/container/span.hh"
#include "cc/bzd/container/array.hh"
#include "cc/bzd/utility/constexpr_for.hh"
#include "cc/bzd/utility/min.hh"
#include "cc/bzd/utility/in_place.hh"

#include <iostream>

namespace bzd
{

/// Spans is a container for span(s), representing data from multiple contiguous memory
/// sections.
/// The knowledge of number of spans is defined at compile time, from which iterators and channels
/// can take advantage for copying or moving data.
template <class T, SizeType N>
class Spans
{
public: // Traits.
	static constexpr SizeType nbSpans = N;
	using Self = Spans<T, N>;

public: // Constructors/assignments.
	constexpr Spans() noexcept = default;
	constexpr Spans(const Self&) noexcept = default;
	constexpr Self& operator=(const Self&) noexcept = default;
	constexpr Spans(Self&&) noexcept = default;
	constexpr Self& operator=(Self&&) noexcept = default;

	template <class... Args>
	constexpr Spans(InPlace, Args&&... args) noexcept : spans_{inPlace, forward<Args>(args)...}
	{
	}

public:
	/// Get the underlying container. 
	[[nodiscard]] constexpr auto& array() const noexcept
	{
		return spans_;
	}

	/// Get the size of the memory sequence.
	/// This is adding all span sizes and returning the sum.
	///
	/// \return The sum of all span sizes.
	[[nodiscard]] constexpr SizeType size() const noexcept
	{
		SizeType sum = 0;
		constexprForContainerInc(spans_, [&](const auto& span) { sum += span.size(); });
		return sum;
	}

	/// Create a sub view of this spans.
	constexpr auto subSpans(const SizeType offset = 0, const SizeType count = npos) const noexcept
	{
		Spans<T, N> sub{};

		SizeType currentOffset = offset;
		SizeType currentCount = count;
		SizeType index = 0;

		// Fill the first part of the span
		while (index < spans_.size() && currentCount)
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
			++index;
		}

		return sub;
	}

private:
	template <class U, SizeType M>
	friend class Spans;

	Array<Span<T>, N> spans_{};
};

}
