#pragma once

#include "cc/bzd/type_traits/declval.hh"
#include "cc/bzd/type_traits/is_trivially_copyable.hh"
#include "cc/bzd/type_traits/iterator.hh"
#include "cc/bzd/utility/begin.hh"
#include "cc/bzd/utility/end.hh"
#include "cc/bzd/utility/size.hh"

namespace bzd::concepts {

template <class T>
concept range = requires(T& t) {
					bzd::begin(t);
					bzd::end(t);
				};

template <class T>
concept sizedRange = range<T> && requires(T& t) { bzd::size(t); };

} // namespace bzd::concepts

namespace bzd::typeTraits {

template <class T>
struct Range;

template <concepts::range T>
struct Range<T>
{
	using Iterator = decltype(bzd::begin(bzd::typeTraits::declval<T&>()));
	using Sentinel = decltype(bzd::end(bzd::typeTraits::declval<T&>()));
	// using Size = decltype(bzd::size(bzd::typeTraits::declval<T&>()));
	using ValueType = typename typeTraits::Iterator<Iterator>::ValueType;
	using DifferenceType = typename typeTraits::Iterator<Iterator>::DifferenceType;
	static constexpr typeTraits::IteratorCategory category = typeTraits::Iterator<Iterator>::category;
};

template <class T>
using RangeIterator = typename Range<T>::Iterator;

template <class T>
using RangeSentinel = typename Range<T>::Sentinel;

template <class T>
using RangeValue = typename Range<T>::ValueType;

template <class T>
using RangeDifference = typename Range<T>::DifferenceType;

template <class T>
inline constexpr IteratorCategory rangeCategory = Range<T>::category;

} // namespace bzd::typeTraits

namespace bzd::concepts {

/// Check that a range satisfies a specific iterator category.
template <class T, typeTraits::IteratorCategory category>
concept rangeCategory = ((typeTraits::rangeCategory<T> & category) == category);

template <class T>
concept inputOrOutputRange = range<T> && inputOrOutputIterator<typeTraits::RangeIterator<T>>;

template <class T>
concept inputRange = inputOrOutputRange<T> && rangeCategory<T, typeTraits::IteratorCategory::input>;

template <class T>
concept outputRange = inputOrOutputRange<T> && rangeCategory<T, typeTraits::IteratorCategory::output>;

template <class T>
concept forwardRange = inputRange<T> && rangeCategory<T, typeTraits::IteratorCategory::forward>;

template <class T>
concept bidirectionalRange = forwardRange<T> && rangeCategory<T, typeTraits::IteratorCategory::bidirectional>;

template <class T>
concept randomAccessRange = bidirectionalRange<T> && rangeCategory<T, typeTraits::IteratorCategory::randomAccess>;

template <class T>
concept contiguousRange = randomAccessRange<T> && rangeCategory<T, typeTraits::IteratorCategory::contiguous>;

template <class T>
concept streamRange = inputOrOutputRange<T> && rangeCategory<T, typeTraits::IteratorCategory::stream>;

template <class T>
concept byteCopyableRange = range<T> && sizeof(typeTraits::RangeValue<T>) == 1u && concepts::triviallyCopyable<typeTraits::RangeValue<T>>;

template <class T>
concept inputStreamRange = streamRange<T> && byteCopyableRange<T> && inputRange<T>;

template <class T>
concept outputStreamRange = streamRange<T> && byteCopyableRange<T> && outputRange<T>;

} // namespace bzd::concepts
