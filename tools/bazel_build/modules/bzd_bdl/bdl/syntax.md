# BDL Syntax

## Builtins

| Name           | Description                                                                                  |
| -------------- | -------------------------------------------------------------------------------------------- |
| `Void`         | Represents an empty value.                                                                   |
| `Integer`      | Any integer value, positive or negative.                                                     |
| `Float`        | Any floating point value, positive or negative.                                              |
| `Boolean`      |                                                                                              |
| `Byte`         | An 8-bit integer value.                                                                      |
| `Span<T>`      | A non-owning type that refers to a contiguous sequence of arbitrary data type.               |
| `Vector<T>`    | An owning type that holds a contiguous variable sequence of elements of arbitrary data type. |
| `Result<V, E>` |                                                                                              |

## Contracts

### integer

Assert the value is an integer.

ex: `[integer]`

## component

Must be named.

Accepts only `config` nested elements, from which only `expression`(s) and `using`(s) are allowed.

`expression`(s) are used to define the list of values the constructor takes.
`using`(s) are used to define the list of template types the component type takes.

Example:

```bdl
component MyComponent
{
config:
    using T = Any;
    size = Integer [min(1)];
    value = Float;
}

composition
{
    MyComponent<AType>(size = 23, value = 3.14);
}
```

This element will be translated in C++ to `MyComponent<AType, 23>{3.14}`
