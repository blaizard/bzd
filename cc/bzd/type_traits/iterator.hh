#pragma once

#include "cc/bzd/type_traits/derived_from.hh"
#include "cc/bzd/type_traits/is_base_of.hh"
#include "cc/bzd/type_traits/is_pointer.hh"
#include "cc/bzd/type_traits/remove_pointer.hh"
#include "cc/bzd/type_traits/underlying_type.hh"

namespace bzd::typeTraits {

/// Base class for all iterators.
struct IteratorBase
{
};

/// Categories for the iterator concept.
///
/// Add multi dimensions hierarchy to iterator category concept, this is a problem already pointed, see:
/// https://www.boost.org/doc/libs/1_40_0/libs/iterator/doc/new-iter-concepts.html
enum class IteratorCategory
{
	/// An input iterator is an iterator that can read from the pointed-to element.
	/// Input iterators only guarantee validity for single pass algorithms: once an input iterator i has been incremented,
	/// all copies of its previous value may be invalidated.
	input = 0x01,
	/// An output iterator is an iterator that can write to the pointed-to element.
	/// Similarly to the input iterator, all copies of its previous value may be invalidated.
	output = 0x02,
	/// A forward iterator is an iterator that can read data from the pointed-to element.
	/// Unlike an input iterator and output iterator, it can be used in multipass algorithms since it
	/// guarantees that two iterators to the same range can be compared against each other.
	forward = 0x04 | input | output,
	/// A bidirectional iterator is an iterator that can be moved in both directions (i.e. incremented and decremented).
	bidirectional = 0x08 | forward,
	/// A random access iterator is an iterator that can be moved to point to any element in constant time.
	randomAccess = 0x10 | bidirectional,
	/// A contiguous iterator is a random access iterator that provides a guarantee that its elements are stored contiguously in the memory.
	contiguous = 0x20 | randomAccess,
	/// A stream iterator is a single pass iterator that pushes the concept further than input or output,
	/// by enforcing this onto its associated range. Therefore this concept applies to ranges more than iterators.
	stream = 0x40
};

/// Bitwise or operator to IteratorCategory.
constexpr IteratorCategory operator|(const IteratorCategory left, const IteratorCategory right) noexcept
{
	return IteratorCategory{static_cast<UnderlyingType<IteratorCategory>>(left) | static_cast<UnderlyingType<IteratorCategory>>(right)};
}

/// Bitwise and operator to IteratorCategory.
constexpr IteratorCategory operator&(const IteratorCategory left, const IteratorCategory right) noexcept
{
	return IteratorCategory{static_cast<UnderlyingType<IteratorCategory>>(left) & static_cast<UnderlyingType<IteratorCategory>>(right)};
}

template <class>
struct Iterator;

template <concepts::derivedFrom<IteratorBase> T>
struct Iterator<T>
{
	static constexpr auto category = T::category;
	using DifferenceType = typename T::DifferenceType;
	using ValueType = typename T::ValueType;
};

template <concepts::pointer T>
struct Iterator<T>
{
	static constexpr auto category = typeTraits::IteratorCategory::contiguous;
	using DifferenceType = decltype(bzd::typeTraits::declval<T>() - bzd::typeTraits::declval<T>());
	using ValueType = typeTraits::RemovePointer<T>;
};

template <class T>
using IteratorValue = typename Iterator<T>::ValueType;

template <class T>
using IteratorDifference = typename Iterator<T>::DifferenceType;

template <class T>
inline constexpr IteratorCategory iteratorCategory = Iterator<T>::category;

} // namespace bzd::typeTraits

namespace bzd::concepts {

template <class T>
concept iterator = concepts::derivedFrom<T, typeTraits::IteratorBase> || concepts::pointer<T>;

/// Check that an iterator satisfies a specific iterator category.
template <class T, typeTraits::IteratorCategory category>
concept iteratorCategory = ((typeTraits::iteratorCategory<T> & category) == category);

/// An input or output iterator must satisfy the following operations:
/// - *x : retreive the value.
/// - ++x : increment
template <class T>
concept inputOrOutputIterator = iterator<T> && requires(T t) {
												   *t;
												   ++t;
											   };

/// One-pass iterator. Does not change the value of a container in other word, this is a read-only iterator.
template <class T>
concept inputIterator = inputOrOutputIterator<T> && iteratorCategory<T, typeTraits::IteratorCategory::input>;

/// One-pass iterator. Cannot be read from, only written to.
template <class T>
concept outputIterator = inputOrOutputIterator<T> && iteratorCategory<T, typeTraits::IteratorCategory::output>;

/// Multi-pass iterator.
template <class T>
concept forwardIterator = inputIterator<T> && iteratorCategory<T, typeTraits::IteratorCategory::forward> && requires(T t) { t++; };

template <class T>
concept bidirectionalIterator = forwardIterator<T> && iteratorCategory<T, typeTraits::IteratorCategory::bidirectional>;

template <class T>
concept randomAccessIterator = bidirectionalIterator<T> && iteratorCategory<T, typeTraits::IteratorCategory::randomAccess>;

template <class T>
concept contiguousIterator = randomAccessIterator<T> && iteratorCategory<T, typeTraits::IteratorCategory::contiguous>;

} // namespace bzd::concepts
