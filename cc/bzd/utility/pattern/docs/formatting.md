# Formatting (toString / toStream)

The pattern library provides two synchronous and asynchronous ways to format values into an output:

| Entry point     | Sink                          | Nature           |
| --------------- | ----------------------------- | ---------------- |
| `bzd::toString` | any `outputByteCopyableRange` | constexpr, sync  |
| `bzd::toStream` | a `bzd::OStream`              | coroutine, async |

Both follow the same [pattern string syntax](syntax.md).

## toString

`bzd::toString` formats arguments into any output byte range. It is `constexpr` and returns
the number of bytes written on success.

```c++
#include "cc/bzd/utility/pattern/to_string.hh"

bzd::String<128> str;
const auto result = bzd::toString(str.assigner(), "Hello {}, you are {} years old"_csv, name, age);
if (result)
{
	// result.value() bytes written, str now contains the formatted text.
}
```

Per-type format options (integral, floating point, string, pointer, enum, etc.) are documented
in the [pattern string syntax](syntax.md). Nested pattern strings are also supported as
formatting arguments.

## toStream

`bzd::toStream` formats arguments into a `bzd::OStream`. It is a coroutine and returns the number
of bytes written, propagating errors with the `!` operator.

```c++
#include "cc/bzd/utility/pattern/to_stream.hh"

bzd::StringStream<N> stream;
co_await bzd::toStream(stream, "Hello {}"_csv, name);
```

Asynchronous byte ranges (async generators) are also supported as arguments.
