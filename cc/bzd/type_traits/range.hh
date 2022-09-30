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
	using DifferenceType = typename typeTraits::Iterator<Iterator>::DifferenceType;
	using ValueType = typename typeTraits::Iterator<Iterator>::ValueType;
};

} // namespace bzd::typeTraits

namespace bzd::concepts {

template <class T>
concept inputRange = range<T> && derivedFrom<typename typeTraits::Range<T>::Category, typeTraits::InputTag>;

template <class T>
concept outputRange = range<T> && derivedFrom<typename typeTraits::Range<T>::Category, typeTraits::OutputTag>;

template <class T>
concept forwardRange = range<T> && derivedFrom<typename typeTraits::Range<T>::Category, typeTraits::ForwardTag>;

template <class T>
concept bidirectionalRange = range<T> && derivedFrom<typename typeTraits::Range<T>::Category, typeTraits::BidirectionalTag>;

template <class T>
concept randomAccessRange = range<T> && derivedFrom<typename typeTraits::Range<T>::Category, typeTraits::RandomAccessTag>;

template <class T>
concept contiguousRange = range<T> && derivedFrom<typename typeTraits::Range<T>::Category, typeTraits::ContiguousTag>;

} // namespace bzd::concepts
