# BDL Syntax

## Contracts

### integer

Assert the value is an integer.

ex: `[integer]`

## component

Must be named.

Accepts only `config` nested elements, from which only `expressions` are allowed.
Components defines the set of input the constructor takes.

3 kinds of expressions can be defined:
- Decorated with `[template type]`: Allow only types.
- Decorated with `[template]`: Allow values only, will be translated into template arguments (in C++).
- Other: Allow values only, are normal expression arguments.

Example:
```bdl
component MyComponent
{
config:
    Any [template type];
    size = Integer [template min(1)];
    value = Float;
}

composition
{
    MyComponent<AType>(size = 23, value = 3.14);
}
```
This element will be trnaslated in C++ to `MyComponent<AType, 23>{3.14}`
