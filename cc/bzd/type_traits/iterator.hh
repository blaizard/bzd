#pragma once

#include "cc/bzd/type_traits/derived_from.hh"
#include "cc/bzd/type_traits/is_base_of.hh"
#include "cc/bzd/type_traits/is_pointer.hh"
#include "cc/bzd/type_traits/remove_pointer.hh"

namespace bzd::typeTraits {

/// Base class for all iterators.
struct IteratorBase
{
};

/// An input iterator is an iterator that can read from the pointed-to element.
/// Input iterators only guarantee validity for single pass algorithms: once an input iterator i has been incremented,
/// all copies of its previous value may be invalidated.
struct InputTag
{
};

/// An output iterator is an iterator that can write to the pointed-to element.
struct OutputTag
{
};

/// A forward iterator is an iterator that can read data from the pointed-to element.
/// Unlike an input iterator and output iterator, it can be used in multipass algorithms.
struct ForwardTag
	: public InputTag
	, public OutputTag
{
};

/// A bidirectional iterator is an iterator that can be moved in both directions (i.e. incremented and decremented).
struct BidirectionalTag : public ForwardTag
{
};

/// A random access iterator is an iterator that can be moved to point to any element in constant time.
struct RandomAccessTag : public BidirectionalTag
{
};

/// A contiguous iterator is a random access iterator that provides a guarantee that its elements are stored contiguously in the memory.
struct ContiguousTag : public RandomAccessTag
{
};

template <class>
struct Iterator;

template <concepts::derivedFrom<IteratorBase> T>
struct Iterator<T>
{
	using Category = typename T::Category;
	using DifferenceType = typename T::DifferenceType;
	using ValueType = typename T::ValueType;
};

template <concepts::pointer T>
struct Iterator<T>
{
	using Category = typeTraits::ContiguousTag;
	using DifferenceType = decltype(bzd::typeTraits::declval<T>() - bzd::typeTraits::declval<T>());
	using ValueType = typeTraits::RemovePointer<T>;
};

template <class T>
using IteratorCategory = typename Iterator<T>::Category;

template <class T>
using IteratorValue = typename Iterator<T>::ValueType;

template <class T>
using IteratorDifference = typename Iterator<T>::DifferenceType;

} // namespace bzd::typeTraits

namespace bzd::concepts {

template <class T>
concept iterator = concepts::derivedFrom<T, typeTraits::IteratorBase> || concepts::pointer<T>;

/// An input or output iterator must satisfy the following operations:
/// - *x : retreive the value.
/// - ++x : increment
/// - x++ : increment
template <class T>
concept inputOrOutputIterator = iterator<T> && requires(T t)
{
	*t;
	++t;
	t++;
};

/// One pass iterator. Does not change the value of a container in other word, this is a read-only iterator.
template <class T>
concept inputIterator = inputOrOutputIterator<T> && derivedFrom<typeTraits::IteratorCategory<T>, typeTraits::InputTag>;

/// One pass iterator. Can be read from, only written to.
template <class T>
concept outputIterator = inputOrOutputIterator<T> && derivedFrom<typeTraits::IteratorCategory<T>, typeTraits::OutputTag>;

template <class T>
concept forwardIterator = inputIterator<T> && derivedFrom<typeTraits::IteratorCategory<T>, typeTraits::ForwardTag>;

template <class T>
concept bidirectionalIterator = forwardIterator<T> && derivedFrom<typeTraits::IteratorCategory<T>, typeTraits::BidirectionalTag>;

template <class T>
concept randomAccessIterator = bidirectionalIterator<T> && derivedFrom<typeTraits::IteratorCategory<T>, typeTraits::RandomAccessTag>;

template <class T>
concept contiguousIterator = randomAccessIterator<T> && derivedFrom<typeTraits::IteratorCategory<T>, typeTraits::ContiguousTag>;

} // namespace bzd::concepts
