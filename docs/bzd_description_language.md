# Bzd Description Language (bdl)

The following describes the the Bzd Description Language (BDL) used to describe interfaces to communicate between components.

## Use Cases

Provide an interface to communicate within the same application between components.
Ability to communication between applications and programming languages.

Also, interfaces should be described once but the possibility to generate code in the different
language reduces the possibility of errors and reduces the amount of boiler plate code.

Enforce contracts for input and outputs of the components.

For example:
- Describe API for REST-based communication between client and server.
- Describe interface for C++ drivers.

## Types

### Built-in Types

The following built-in types are made available by the implementation as built-in types.

- Boolean
- Integer
- Float
- String
- Optional<T>
- Result<T, E>
- Async<T, E>
- Sequence<T>
- Tuple<T, U, ...>
- Variant<T, U, ...>
- Callable

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
[const] <type> <name> [= <value>] [[<constraints>...]];
```

## Methods

A function is defined as follow:
```
method <name>(<variable1>, ...) [-> <type>] [[<constraints>...]];
```

## Interfaces

Interfaces are pure virtual classes that are defined with the keywork `interface`.
It contains children which can be either functions or variables.
