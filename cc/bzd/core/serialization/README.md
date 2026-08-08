# Serialization

Binary serialization of built-in types to and from byte ranges.

## Key Points

- **Deterministic** - data is always serialized in little-endian byte order.
- **Extensible** - custom types can be supported by specializing `bzd::Serialization<T>`.

## Usage

```c++
bzd::String<16> buffer{};

bzd::serialize(buffer.assigner(), value);
// ...
bzd::deserialize(buffer, value);
```
