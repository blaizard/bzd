#pragma once

#include "cc/bzd/type_traits/declval.hh"
#include "cc/bzd/type_traits/iterator.hh"
#include "cc/bzd/utility/begin.hh"
#include "cc/bzd/utility/end.hh"
#include "cc/bzd/utility/size.hh"

namespace bzd::concepts {

template <class T>
concept range = requires(T t)
{
	bzd::begin(t);
	bzd::end(t);
};

} // namespace bzd::concepts

namespace bzd::typeTraits {

template <class T>
requires concepts::range<T>
struct Iterator<T>
{
	using Type = decltype(bzd::begin(bzd::typeTraits::declval<T&>()));
	using Category = typename Iterator<Type>::Category;
	using DifferenceType = typename Iterator<Type>::DifferenceType;
	using ValueType = typename Iterator<Type>::ValueType;
};

} // namespace bzd::typeTraits

namespace bzd::concepts {

template <class T>
concept inputRange = range<T> && derivedFrom<typename typeTraits::Iterator<T>::Category, typeTraits::InputTag>;

template <class T>
concept outputRange = range<T> && derivedFrom<typename typeTraits::Iterator<T>::Category, typeTraits::OutputTag>;

template <class T>
concept forwardRange = range<T> && derivedFrom<typename typeTraits::Iterator<T>::Category, typeTraits::ForwardTag>;

template <class T>
concept bidirectionalRange = range<T> && derivedFrom<typename typeTraits::Iterator<T>::Category, typeTraits::BidirectionalTag>;

template <class T>
concept randomAccessRange = range<T> && derivedFrom<typename typeTraits::Iterator<T>::Category, typeTraits::RandomAccessTag>;

template <class T>
concept contiguousRange = range<T> && derivedFrom<typename typeTraits::Iterator<T>::Category, typeTraits::ContiguousTag>;

template <class T>
concept sizedRange = range<T> && requires(T t)
{
	bzd::size(t);
};

} // namespace bzd::concepts
