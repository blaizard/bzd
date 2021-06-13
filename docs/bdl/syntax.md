# Bdl Syntax

The following describes the the Bzd Description Language (BDL) used to describe interfaces to communicate between components.

## Use Cases

The following use cases are coverd by this description language:
- Provide an inter-core/inter-language communication mean between components.
- Reduce boiler plate for interfaces.
- Enfore input/output constraints (contracts).

For example:
- Describe API for REST-based communication between client and server.
- Describe interface for C++ drivers.

## Types

### Built-in Types

The following built-in types are made available by the implementation as built-in types.

- Boolean
- Integer (*)
- Float (*)
- String
- Optional<T>
- Result<T, E>
- Async<T, E>
- Sequence<T>
- Tuple<T, U, ...>
- Variant<T, U, ...>
- Callable

(*) These types cannot be used directly and need to be used through a `using` statement to enforce strong typing.

### Structures

```
struct Coord
{
	Float x;
	Float y;
}
```

### Enumeration
```
enum MyEnum
{
	FIRST,
	SECOND
}
```

### Strong Types

Strong typing is supported with the `using` keyword. For example, to define an integer strong type, this can be done as follow:
```
using MyStrongType = Integer [min = 0, max = 23];
```

## Namespaces

To prevent symbol leakage, files can be defined to be under a specific namespace, with the following keyword:
```
namespace bzd.dummy;
```
In this example, all entities declared in this file will be accessed prepended by the `bzd.dummy` namespace.

## Dependencies

All BDL dependencies should be imported at the top of the file, referenced by their path.
```
import "cc/bzd/core/my_interface.bdl"
```

## Variables

A variable is defined as follow:
```
[const] <type> <name> [= <value>] [[<contracts>...]];
```

## Methods

A function is defined as follow:
```
method <name>(<variable1>, ...) [-> <type>] [[<contracts>...]];
```

## Interfaces

Interfaces are pure virtual classes that are defined with the keywork `interface`.
It contains children which can be either functions or variables.

## Module

A module is the implementation of one or multiple interfaces. It can be instantiated and exposes
the functionality of the interface it inherits from.
In addition it expose things like configuration details, dependencies... to the description language.

For example:
```
namespace bzd.platform.impl;

module MyModule : bzd.platform.Reader
{
config:
	Integer channel [template];
}
```

