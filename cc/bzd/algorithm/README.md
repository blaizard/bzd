# Algorithm

A collection of `constexpr`, allocation-free algorithms operating on ranges (iterators or range objects).

## Algorithms

- `allOf` - Checks if a predicate holds for all elements of a range.
- `anyOf` - Checks if a predicate holds for at least one element of a range.
- `noneOf` - Checks if a predicate holds for no element of a range.
- `find` - Finds the first element equal to a value.
- `findIf` - Finds the first element satisfying a predicate.
- `findIfNot` - Finds the first element not satisfying a predicate.
- `search` - Searches for the first occurrence of a sub-range.
- `rsearch` - Searches for the last occurrence of a sub-range.
- `copy` - Copies elements from one range to another.
- `copyN` - Copies exactly N elements from one range to another.
- `copyBackward` - Copies elements in reverse order, preserving their relative order.
- `byteCopy` - Copies elements from one range to another at byte level.
- `move` - Moves elements from one range to another.
- `moveBackward` - Moves elements in reverse order, preserving their relative order.
- `fill` - Assigns a value to all elements of a range.
- `fillN` - Assigns a value to the first N elements of a range.
- `equal` - Checks if two ranges are equal.
- `lexicographicalCompare` - Compares two ranges lexicographically.
- `lowerBound` - Finds the first element not less than a value in a sorted range.
- `upperBound` - Finds the first element greater than a value in a sorted range.
- `equalRange` - Finds the sub-range of elements equivalent to a value in a sorted range.
- `binarySearch` - Checks if a value is present in a sorted range.
- `sort` - Sorts a range using a heap sort.
- `reverse` - Reverses the order of elements in a range.
- `startsWithAnyOf` - Checks if a range starts with any of a set of ranges.
