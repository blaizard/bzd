# Custom Specializations

The pattern library is extensible: any user-defined type can be formatted or parsed by
specializing one of the four base templates.

| Base template        | Purpose                            | Nature           |
| -------------------- | ---------------------------------- | ---------------- |
| `bzd::ToString<T>`   | Format `T` into a byte range.      | constexpr, sync  |
| `bzd::ToStream<T>`   | Format `T` into a `bzd::OStream`.  | coroutine, async |
| `bzd::FromString<T>` | Parse `T` from a byte range.       | constexpr, sync  |
| `bzd::FromStream<T>` | Parse `T` from an async generator. | coroutine, async |

## Why Specializations Are Classes

Specializations are classes rather than overloaded functions to support partial
specialization. Recursive function overloading forces a definition order that triggers
"specialization of [...] after instantiation" errors. See
[GotW #17](http://www.gotw.ca/publications/mill17.htm).

## How the Framework Finds a Specialization

Built-in schemas map a value type to its specialization through `typeTraits::ToString<T>`
(which resolves to `bzd::ToString<RemoveCVRef<T>>`), and similarly for the three other
templates. This is why specializing `bzd::ToString<MyType>` is sufficient.

## The Process Function

Each specialization must provide a static `process` function. The framework calls it once
per replacement field, with the output range (or stream), the value and the parsed metadata.

A `ToString` specialization:

```c++
template <>
struct bzd::ToString<MyType>
{
	template <concepts::outputByteCopyableRange Range>
	static constexpr Optional<Size> process(Range&& range, const MyType& value) noexcept
	{
		// Write the value into range, return the number of bytes written.
		// Return bzd::nullopt on failure.
	}
};
```

A `FromString` specialization:

```c++
template <>
struct bzd::FromString<MyType>
{
	template <bzd::concepts::inputByteCopyableRange Range>
	static constexpr Optional<Size> process(Range&& range, MyType& value) noexcept
	{
		// Read from range into value, return the number of bytes consumed.
		// Return bzd::nullopt if the input does not match.
	}
};
```

The async variants (`ToStream`/`FromStream`) have the same shape but return
`bzd::Async<Size>` and take a `bzd::OStream&` or an async generator respectively.

## Metadata

A specialization can accept format options by declaring a `Metadata` type and a `parse`
function. The `Adapter` reports invalid options at compile time (when the pattern is a
compile-time string).

```c++
template <>
struct bzd::ToString<MyType>
{
	/// The type used to store the parsed options.
	struct Metadata
	{
		Bool isUpperCase = false;
	};

	/// Parse the option string, this is only evaluated at compile time.
	template <class Adapter>
	static constexpr Metadata parse(const bzd::StringView options) noexcept
	{
		Metadata metadata{};
		for (const auto c : options)
		{
			switch (c)
			{
			case 'u':
				metadata.isUpperCase = true;
				break;
			default:
				Adapter::onError("Unsupported option.");
			}
		}
		return metadata;
	}

	template <concepts::outputByteCopyableRange Range>
	static constexpr Optional<Size> process(Range&& range, const MyType& value, const Metadata metadata = Metadata{}) noexcept
	{
		...
	}
};
```

The pattern `"{:u}"_csv` parses `u` through `parse` and passes the resulting `Metadata` to
`process`.

## Using a Custom Schema

A user-defined schema maps a value type to its specialization, to be passed to the
low-level entry points.

```c++
class MySchema
{
public:
	template <class Value>
	using Specialization = typename typeTraits::template ToString<Value>;
};
```

## Requirements

- The `process` function must be `static` and `constexpr` (async variants are coroutines).
- If a `Metadata` type is declared, a `parse` function is required.
- No dynamic allocation: use fixed-size containers for any internal buffer.
