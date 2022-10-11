#pragma once

#include "cc/bzd/type_traits/declval.hh"
#include "cc/bzd/type_traits/iterator.hh"
#include "cc/bzd/utility/begin.hh"
#include "cc/bzd/utility/end.hh"
#include "cc/bzd/utility/size.hh"

namespace bzd::concepts {

template <class T>
concept range = requires(T& t)
{
	bzd::begin(t);
	bzd::end(t);
};

template <class T>
concept sizedRange = range<T> && requires(T& t)
{
	bzd::size(t);
};

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
	using Category = typename typeTraits::Iterator<Iterator>::Category;
	using ValueType = typename typeTraits::Iterator<Iterator>::ValueType;
	using DifferenceType = typename typeTraits::Iterator<Iterator>::DifferenceType;
};

template <class T>
using RangeIterator = typename Range<T>::Iterator;

template <class T>
using RangeSentinel = typename Range<T>::Sentinel;

template <class T>
using RangeCategory = typename Range<T>::Category;

template <class T>
using RangeValue = typename Range<T>::ValueType;

template <class T>
using RangeDifference = typename Range<T>::DifferenceType;

} // namespace bzd::typeTraits

namespace bzd::concepts {

template <class T>
concept inputRange = range<T> && derivedFrom<typeTraits::RangeCategory<T>, typeTraits::InputTag>;

template <class T>
concept outputRange = range<T> && derivedFrom<typeTraits::RangeCategory<T>, typeTraits::OutputTag>;

template <class T>
concept forwardRange = range<T> && derivedFrom<typeTraits::RangeCategory<T>, typeTraits::ForwardTag>;

template <class T>
concept bidirectionalRange = range<T> && derivedFrom<typeTraits::RangeCategory<T>, typeTraits::BidirectionalTag>;

template <class T>
concept randomAccessRange = range<T> && derivedFrom<typeTraits::RangeCategory<T>, typeTraits::RandomAccessTag>;

template <class T>
concept contiguousRange = range<T> && derivedFrom<typeTraits::RangeCategory<T>, typeTraits::ContiguousTag>;

} // namespace bzd::concepts
