#pragma once

#include "cc/bzd/type_traits/is_base_of.hh"
#include "cc/bzd/type_traits/is_pointer.hh"
#include "cc/bzd/type_traits/remove_pointer.hh"
#include "cc/bzd/utility/concept.hh"

namespace bzd::iterator {

/// Base class for all iterators.
struct Iterator
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
struct ForwardTag : public InputTag
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

template <class T, class = void>
struct Traits;

template <class T>
struct Traits<T, REQUIRES_SPECIALIZATION(typeTraits::isBaseOf<Iterator, T>)>
{
	using Category = typename T::Category;
	using DifferenceType = typename T::DifferenceType;
	using ValueType = typename T::ValueType;
};

template <class T>
struct Traits<T, REQUIRES_SPECIALIZATION(typeTraits::isPointer<T>)>
{
	using Category = ContiguousTag;
	using DifferenceType = bzd::Int32Type;
	using ValueType = typeTraits::RemovePointer<T>;
};

template <class T, class Category>
CONCEPT isCategory = typeTraits::isBaseOf<Category, typename Traits<T>::Category>;

} // namespace bzd::iterator
