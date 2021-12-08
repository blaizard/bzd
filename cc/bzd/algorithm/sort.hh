#pragma once

#include "cc/bzd/container/iterator/distance.hh"
#include "cc/bzd/type_traits/remove_cvref.hh"
#include "cc/bzd/utility/comparison/less.hh"
#include "cc/bzd/utility/swap.hh"

/// This implementation uses heap sort (n*log(n) complexity), sort elements in-place,
/// and do not require extra memory (stack or other) to process.
/// The implementation is inspired by: https://www.geeksforgeeks.org/iterative-heap-sort/

namespace bzd::algorithm {

namespace impl {
/// Build a max heap where value of each child is always smaller
/// than value of their parent.
template <class Iterator, class Compare>
constexpr void makeHeap(Iterator begin, Iterator end, const Compare& comparison) noexcept
{
	const auto size = bzd::iterator::distance(begin, end);
	using IndexType = typeTraits::RemoveCVRef<decltype(size)>;

	for (IndexType i = 1; i < size; i++)
	{
		// If child is bigger than parent.
		if (comparison(begin[(i - 1) / 2], begin[i]))
		{
			IndexType j = i;

			// Swap child and parent until parent is smaller.
			while (comparison(begin[(j - 1) / 2], begin[j]))
			{
				bzd::swap(begin[j], begin[(j - 1) / 2]);
				j = (j - 1) / 2;
			}
		}
	}
}
} // namespace impl

template <class Iterator, class Compare = bzd::Less<typename Iterator::ValueType>>
constexpr void sort(Iterator begin, Iterator end, const Compare comparison = Compare{}) noexcept
{
	const auto size = bzd::iterator::distance(begin, end);
	using IndexType = typeTraits::RemoveCVRef<decltype(size)>;

	impl::makeHeap(begin, end, comparison);

	for (IndexType i = size - 1; i > 0; i--)
	{
		// Swap value of first indexed with last indexed.
		bzd::swap(begin[0], begin[i]);

		// Maintaining heap property after each swapping.
		IndexType j{0};
		IndexType index{0};

		do
		{
			index = (2 * j + 1);

			// If left child is smaller than right child point index variable to right child.
			if (index < (i - 1) && comparison(begin[index], begin[index + 1]))
			{
				index++;
			}

			// If parent is smaller than child then swapping parent with child having higher value.
			if (index < i && comparison(begin[j], begin[index]))
			{
				bzd::swap(begin[j], begin[index]);
			}

			j = index;

		} while (index < i);
	}
}

} // namespace bzd::algorithm
