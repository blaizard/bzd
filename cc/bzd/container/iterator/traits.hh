#pragma once

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

} // namespace bzd::iterator
