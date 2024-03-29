#pragma once

#include "cc/bzd/type_traits/iterator.hh"
#include "cc/bzd/type_traits/predicate.hh"
#include "cc/bzd/type_traits/range.hh"
#include "cc/bzd/type_traits/sentinel_for.hh"
#include "cc/bzd/utility/forward.hh"

namespace bzd::algorithm {

/// Returns an iterator to the first element in the range [first, last) for which predicate \c predicate returns \c true.
///
/// \param[in] first The beginning of the range of elements to examine.
/// \param[in] last The ending of the range of elements to examine.
/// \param[in] predicate The unary predicate which returns \c ​true for the required element.
template <concepts::forwardIterator Iterator,
		  concepts::sentinelFor<Iterator> Sentinel,
		  concepts::predicate<typeTraits::IteratorValue<Iterator>> UnaryPredicate>
[[nodiscard]] constexpr Iterator findIf(Iterator first, Sentinel last, UnaryPredicate predicate) noexcept
{
	for (; first != last; ++first)
	{
		if (predicate(*first))
		{
			return first;
		}
	}
	return last;
}

/// \copydoc findIf
/// \param[in,out] range The range of elements to examine.
template <concepts::forwardRange Range, class... Args>
constexpr auto findIf(Range&& range, Args&&... args)
{
	return bzd::algorithm::findIf(bzd::begin(range), bzd::end(range), bzd::forward<Args>(args)...);
}

} // namespace bzd::algorithm
