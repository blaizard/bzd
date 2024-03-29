# Entities

An entity is a mirror of its underlying element. It simply presents the information in a simpler way.

All entities have the following common attributes:

- _category_: The category of the entity, `expression`, `struct`, etc... This is used to select the `Entity` wrapper class and convert
  a serializable `Element` into an `Entity`.
- _fqn_: The Fully Qualified Name. This corresponds to a unique identifier that corresponds to this specific entity. There
  cannot be more than one entity with the same FQN.
  FQN is a succession of types separated with ';'. For example a `struct A { c = Integer; }` in a `namespace B` will look like: `B.A`.
  When instantiated in a composition as such: `composition { a = B.A; a.c; }`, The FQN of `a.c` will be `a.c` but its value FQN is `a;A.c`.

## Expression

The following is what an epxression looks like:

```bdl
name : interface = Type<template>(parameters) [contracts]
```

Expressions instantiate and/or assign a value to a variable (`name`). Therefore they only hold values, they can instantiate a new value from a type
or copy an existing value.

_name_: This is the name of the instance. It is used to refer to this object locally or globally.
_interface_: This is the interface exposed by this name. In other word, when using this object, the user will only see the
interface and not the actual object.
_Type<template>_: The type of this object.
_contracts_: The contracts associated with this expression.
