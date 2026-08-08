# BDL C++ backend

The C++ extension for the BDL (Bzd Description Language) framework. It provides the Bazel rules and code generators that turn BDL definitions into C++ code.

## Key Points

- **C++ code generation** - generates headers from `.bdl` interface and component definitions.
- **Composition to code** - turns BDL compositions into C++ that wires components together.
- **Bazel integration** - custom rules that embed BDL into the C++ build.

## Usage

BDL definitions are consumed by depending on a `bdl_library` target from any `cc_library` or `bzd_cc_library`. Binary and test rules accept `bdls` directly:

```python
bzd_cc_binary(
    name = "app",
    srcs = ["main.cc"],
    bdls = ["app.bdl"],
    deps = ["//cc:bzd"],
)
```

## Library

The C++ generator converts BDL definitions into C++ header files. For each `.bdl` source file, a matching `.hh` header is produced, so `my/path/core.bdl` becomes `my/path/core.hh`:

```c++
#include "my/path/core.hh"
```

### Interfaces

Interfaces are generated as CRTP adapter classes from which the implementation inherits. Trivial types (enums, structs, usings) declared in the BDL file are emitted in the namespace of the file, or nested inside the interface when declared within it.

For example, given the following interface in `bzd/example/interface.bdl`:

```bdl
namespace bzd.example;

interface MyInterface {
	enum Error {
		uninitialized,
		other
	}
	method process() -> Error;
}
```

The generated header provides:

```c++
// Auto-generated from interface.bdl.
namespace bzd::example {

template <class Impl>
class MyInterface
{
public: // Types
	enum class Error : bzd::UInt8
	{
		uninitialized,
		other
	};

public: // Methods
	[[nodiscard]] Error process() noexcept
	{
		return ::bzd::impl::getImplementation(this, &MyInterface::process, &Impl::process)->process();
	}
};

} // namespace bzd::example
```

The implementation inherits from the adapter and provides the methods:

```c++
#include "bzd/example/interface.hh"

class Implementation : public bzd::example::MyInterface<Implementation>
{
public:
	constexpr Error process() noexcept
	{
		return Error::other;
	}
};
```

Interfaces defined in the BDL language are guaranteed to be accessible and callable from any node in the system.

#### External interfaces

Sometimes an interface must be defined in C++ directly, for example to benefit from features not available in the BDL language. This is done with an `extern` declaration:

```bdl
namespace bzd.example;

extern interface MyInterface;
```

The user is then tasked to provide the C++ symbol within the declared namespace. The interface must use the CRTP pattern. To also validate that the implementation signature matches the interface, use the checked helper:

```c++
namespace bzd::example {

template <class Impl>
class MyInterface
{
public:
	constexpr Error process() noexcept
	{
		// Ensures the interface and implementation signatures match, then calls the implementation.
		return bzd::impl::getImplementation(this, &MyInterface::process, &Impl::process)->process();
	}
};

} // namespace bzd::example
```

Virtual interfaces are also supported:

```c++
namespace bzd::example {

class MyInterface
{
public:
	virtual Error process() noexcept = 0;
};

} // namespace bzd::example
```

#### Casting

Objects which implement an interface are cast down to their interface when used in the composition; this is done automatically by the code generator and ensures that the object is only used through its interface.

The code generator specializes the `bzd::Interface` type with the fully qualified name of the interface. It can then be used like this:

```c++
auto& interface = bzd::Interface<"bzd.example.MyInterface">::cast(object);
```
