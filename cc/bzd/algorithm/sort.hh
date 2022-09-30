#pragma once

#include "cc/bzd/type_traits/iterator.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/type_traits/remove_cvref.hh"
#include "cc/bzd/utility/comparison/less.hh"
#include "cc/bzd/utility/distance.hh"
#include "cc/bzd/utility/forward.hh"
#include "cc/bzd/utility/swap.hh"

namespace bzd::algorithm {

namespace impl {
/// Build a max heap where value of each child is always smaller
/// than value of their parent.
template <class Iterator, class Compare>
constexpr void makeHeap(Iterator first, Iterator last, Compare& comparison) noexcept
{
	const auto size = bzd::distance(first, last);
	using IndexType = typeTraits::RemoveCVRef<decltype(size)>;

	for (IndexType i = 1; i < size; i++)
	{
		// If child is bigger than parent.
		if (comparison(first[(i - 1) / 2], first[i]))
		{
			IndexType j = i;

			// Swap child and parent until parent is smaller.
			while (comparison(first[(j - 1) / 2], first[j]))
			{
				bzd::swap(first[j], first[(j - 1) / 2]);
				j = (j - 1) / 2;
			}
		}
	}
}
} // namespace impl

/// Sorts the elements in the range [first, last) in non-descending order. The order of equal elements is not guaranteed to be preserved.
///
/// This implementation uses heap sort (n*log(n) complexity), sort elements in-place
/// and do not require extra memory (stack or other) to process.
/// The implementation is inspired by: https://www.geeksforgeeks.org/iterative-heap-sort/
///
/// \param[in,out] first The beginning of the range of elements to be sorted.
/// \param[in,out] last The ending of the range of elements to be sorted.
/// \param[in] comparison Comparison function object which returns ​true if the first argument is less than (i.e. is ordered before) the
/// second.
template <class Iterator, class Compare = bzd::Less<typename typeTraits::Iterator<Iterator>::ValueType>>
requires concepts::randomAccessIterator<Iterator>
constexpr void sort(Iterator first, Iterator last, Compare comparison = Compare{}) noexcept
{
	const auto size = bzd::distance(first, last);
	using IndexType = typeTraits::RemoveCVRef<decltype(size)>;

	impl::makeHeap(first, last, comparison);

	for (IndexType i = size - 1; i > 0; i--)
	{
		// Swap value of first indexed with last indexed.
		bzd::swap(first[0], first[i]);

		// Maintaining heap property after each swapping.
		IndexType j{0};
		IndexType index{0};

		do
		{
			index = (2 * j + 1);

			// If left child is smaller than right child point index variable to right child.
			if (index < (i - 1) && comparison(first[index], first[index + 1]))
			{
				index++;
			}

			// If parent is smaller than child then swapping parent with child having higher value.
			if (index < i && comparison(first[j], first[index]))
			{
				bzd::swap(first[j], first[index]);
			}

			j = index;

		} while (index < i);
	}
}

/// \copydoc sort
/// \param[in,out] range The brange of elements to be sorted.
template <class Range, class... Args>
requires concepts::randomAccessRange<Range>
constexpr void sort(Range&& range, Args&&... args) noexcept { sort(bzd::begin(range), bzd::end(range), bzd::forward<Args>(args)...); }

} // namespace bzd::algorithm
