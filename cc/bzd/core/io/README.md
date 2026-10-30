# IO

Asynchronous byte buffers used to connect components in a composition. A buffer provides a `Source` (writer side) and a `Sink` (reader side).

## Key Points

- **Async** - readers suspend until data is available.
- **In-place access** - data is stored in a fixed-capacity ring buffer and read directly, without intermediate copies.

## Usage

```c++
bzd::io::Buffer<bzd::Byte, 64, "buffer"> buffer{};
auto source = buffer.makeSource();
auto sink = buffer.makeSink();

// Writer side.
co_await !source.set(value);

// Reader side.
co_await !sink.getNew();
```
