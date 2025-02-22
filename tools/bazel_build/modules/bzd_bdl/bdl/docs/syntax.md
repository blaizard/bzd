# Syntax

The following describes the syntax of the Bzd Description Language (BDL) used to describe interfaces to communicate between components.

## Types

### Built-in Types

The following built-in types are made available by the implementation as built-in types.

| Syntax    | Description                                                                                      |
| --------- | ------------------------------------------------------------------------------------------------ |
| Void      | Empty type, usually used for return statements.                                                  |
| Boolean   | Binary type evaluating to `true` or `false`.                                                     |
| Integer   | Represents an arithmetic integer. Its size can be defined with contracts.                        |
| Float     | Represents an arithmetic floating point. Its size can be defined with contracts.                 |
| Byte      | Type representing a byte.                                                                        |
| String    | Type representing a string.                                                                      |
| Result<T> | Templated type that contains either a value of type `T` or an error.                             |
| Async<T>  | Templated type that contains an asynchronous prommise returning a value of type `T` or an error. |
| Array<T>  | A fixed length array of type `T`. Its capacity may be defined with contracts.                    |
| Vector<T> | A resizable and sequential container of type `T`. Its capacity may be defined with contracts.    |

### Literals

Literals is a notation of a value that may define an arbitrary type, written in the bdl format.
Available literals in bdl are:

- integers: `1`, `-4`, ...
- floats: `0.23`, `-3.14`, ...
- booleans: `true`, `false`
- strings: `"hello"`, ...
- enumeration values: `myenum.value1`

To be implemented: numbers with units.

### Structures

```
struct Coord
{
	x = Float;
	y = Integer;
}
```

### Enumeration

```
enum MyEnum
{
	first,
	second
}
```

### Strong Types

Strong typing is supported with the `using` keyword. For example, to define an integer with strong typing, this can be done as follow:

```
using MyStrongType = Integer [min(0) max(23)];
```

## Namespaces

To prevent symbol leakage, files can be defined to be under a specific namespace, with the following keyword:

```
namespace bzd.dummy;
```

In this example, all entities declared in this file will be accessed prepended by the `bzd.dummy` namespace.
Only one namespace can be declared within a file and the namespace statement must be at the top of the file.

## Dependencies

All BDL dependencies should be imported at the top of the file, referenced by their path.

```
use "cc/bzd/core/my_interface.bdl"
```

## Expressions

An expression is defined as follow:

```
[const] <type> <name> [= <values>] [[<contracts>...]];
```

where `values` can be an association of `value`, `symbol`, `regexpr` or `preset`.

- `value` is either a literal.
- `symbol` is either a relative symbol to the current namespace `abc` or a fully qualified name `this.is.abc`.
- `regexpr` is a regular expression `/[a-z]*/`.
- `preset` is a preset that is replaced by a predefined content. For example `abc = {out};` is equivalent to `abd: bzd.OStream = target.out;`

## Methods

A function is defined as follow:

```
method <name>(<expression1>, ...) [-> <type>] [[<contracts>...]];
```

## Interfaces

Interfaces are used by components to expose their public interfaces, they are defined with the keyword `interface`.
Note that the definition does not imply a specific interface implementation.

## Components

A component is the implementation of one or multiple interfaces. It can be instantiated and exposes
the functionality of the interface it inherits from.
In addition it describes its configuration and compisition to the description language.

### Configuration

A component should tell how it is instancited during composition, this is done under the `config` scope as follow:

```bdl
component MyModule
{
config:
	channel = String;
	capacity = Integer(100) [min(1)];
}
```

The composition will then instantiate this component by requiring a string as its `channel` argument and an optional `capacity` argument.

```bdl
composition {
	module = MyModule(channel = "channel1");
}
```

### Composition

Similarly a predefined composition can be described for a component when instantiated. For example:

```bdl
component MyModule
{
interface:
	method run();
composition:
	this.run();
}
```

This will create a workload from the `run` method when the component `MyModule` is instantiated.
Note that this workload will be a service because it was not explicitly declared by the user.
