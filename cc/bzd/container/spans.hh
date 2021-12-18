#pragma once

#include "cc/bzd/container/array.hh"
#include "cc/bzd/container/iterator/container_of_iterables.hh"
#include "cc/bzd/container/span.hh"
#include "cc/bzd/utility/constexpr_for.hh"
#include "cc/bzd/utility/in_place.hh"
#include "cc/bzd/utility/min.hh"

namespace bzd {

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
	using Iterator = bzd::iterator::ContainerOfIterables<typename Array<Span<T>, N>::Iterator>;
	using ConstIterator = bzd::iterator::ContainerOfIterables<typename Array<Span<T>, N>::ConstIterator>;

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
	[[nodiscard]] constexpr auto& array() const noexcept { return spans_; }

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
	///
	/// \param[in] offset From where to start to copy.
	/// \param[in] count The number of elements to copy.
	/// \return A sub view of the current object.
	constexpr auto subSpans(const SizeType offset, const SizeType count = npos) const noexcept
	{
		Spans<T, N> sub{};

		for (SizeType index = 0, currentCount = count, currentOffset = offset; index < spans_.size() && currentCount; ++index)
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

public: // Iterators
	[[nodiscard]] constexpr auto begin() noexcept { return Iterator{spans_.begin(), spans_.end()}; }
	[[nodiscard]] constexpr auto begin() const noexcept { return ConstIterator{spans_.begin(), spans_.end()}; }
	[[nodiscard]] constexpr auto end() noexcept { return Iterator{spans_.end()}; }
	[[nodiscard]] constexpr auto end() const noexcept { return ConstIterator{spans_.end()}; }

private:
	template <class U, SizeType M>
	friend class Spans;

	Array<Span<T>, N> spans_{};
};

} // namespace bzd
