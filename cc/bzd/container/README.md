# Container

Zero-allocation, fixed-capacity data structures for memory-constrained systems.

## Key Points

- **Zero dynamic allocation** - capacities are fixed at compile time.
- **`constexpr` friendly** - most containers can be used in constant expressions.
- **Minimal footprint** - no virtual dispatch, no STL dependency.

## Containers

- `Array` - Fixed-size array with a standard container interface.
- `Vector` - Fixed-capacity sequence container.
- `String` / `StringView` - Fixed-capacity string and non-owning string view.
- `Span` / `Spans` - Non-owning views over one or several contiguous memory sections.
- `Map` - Fixed-capacity flat map with sorted keys.
- `BTree` - Fixed-capacity B-tree.
- `Optional` - A value that may or may not be present.
- `Result` - A value or an error, used for error propagation.
- `Variant` - Type-safe union of a fixed set of types.
- `Tuple` - Fixed-size collection of heterogeneous values.
- `Queue` / `RingBuffer` - FIFO queue and ring buffer.
- `NonOwningList` - Intrusive doubly-linked list.
- `Pool` - Fixed memory pool of reusable elements.
- `Stack` - Fixed stack buffer with usage estimation.
- `ReferenceWrapper` / `ValueWrapper` / `Wrapper` - Reference and value wrappers.
- `NamedType` - Strong types over primitive types.
- `AnyReference` - Type-erased reference to any object.
- `FunctionRef` - Non-owning callable reference.
- `StringStream` / `OStreamBuffered` - Output streams with fixed-capacity buffering.
