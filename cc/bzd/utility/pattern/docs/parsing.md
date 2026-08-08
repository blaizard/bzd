# Parsing (fromString / fromStream)

The pattern library also provides the reverse operation: matching a byte stream against a
pattern string and extracting values into arguments.

| Entry point       | Source                       | Nature           |
| ----------------- | ---------------------------- | ---------------- |
| `bzd::fromString` | any `inputByteCopyableRange` | constexpr, sync  |
| `bzd::fromStream` | any async byte generator     | coroutine, async |

Both use the same [pattern string syntax](syntax.md).

## fromString

`bzd::fromString` matches each static segment against the input using a regular expression
and parses each replacement field into its corresponding mutable output argument. It returns
the number of bytes consumed, or an empty result if the input does not match.

```c++
#include "cc/bzd/utility/pattern/from_string.hh"

bzd::UInt32 year;
bzd::String<16> month;
const auto result = bzd::fromString(input, "{}:{:d}"_csv, month, year);
```

### Field semantics

| Argument type                            | Behavior                                                                                                                                          |
| ---------------------------------------- | ------------------------------------------------------------------------------------------------------------------------------------------------- |
| Integral                                 | Reads digits of the requested base (`b`, `o`, `d`, `x`, `X`), stopping at the first non-matching character.                                       |
| Byte range / string                      | Captures the longest match of a regexp given as the field option (default `\w+`).                                                                 |
| Sorted range of byte ranges (e.g. a map) | Matches the input against the keys of a sorted collection, returning an iterator to the match. `?` selects lazy matching, otherwise it is greedy. |

### Matching a sorted range of strings

`ToSortedRangeOfRanges` (from `cc/bzd/utility/pattern/from_string/range_of_ranges.hh`) allows
matching against the keys of a sorted collection without scanning it linearly.

```c++
bzd::Map<bzd::String<8>, bzd::UInt8> map{...};
ToSortedRangeOfRanges sorted{map};
const auto result = bzd::fromString(input, "{:?}"_csv, sorted); // lazy matching
if (result && sorted.value() != bzd::end(map))
{
	const auto& key = sorted.value()->first;
}
```

## fromStream

`bzd::fromStream` provides the same parsing capabilities for asynchronous byte generators.
