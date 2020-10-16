# Bzd Description Language

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

The following built-in types should be made available by the implementation.

- Void
- Boolean
- Integer
- Float
- Sting
- Optional<T>
- Result<T, E>
- Sequence<T>
- Variant<T, V, ...>

### Custom

In addition to built-in types, any struct can be used, it should be declared as follow:
```
struct Coord
{
	Float x;
	Float y;
}
```

## Namespaces

To prevent symbol leakage, all modules are automatically in a namespace that is defined by their path.
When referenced, all namespaces are relative to the root of the repository.

For example, an interface (MyInterface) defined under cc/bzd/core/my_interface.bdl would be referenced
as follow:
```
cc.bzd.core.MyInterface
```

## Dependencies

All BDL dependencies should be imported at the top of the file, referenced by their path.
```
import "cc/bzd/core/my_interface.bdl"
```

## Variables

A variable is defined as follow:
```
[mutable] <type> <name> [= <value>] [[<constraints>...]];
```

## Functions

A function is defined as follow:
```
<type> <name>(<variable1>, ...);
```

## Interfaces

Interfaces are pure virtual classes that are defined with the keywork `interface`.
It contains children which can be either functions or variables.