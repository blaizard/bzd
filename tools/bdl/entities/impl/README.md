# Entities

An entity is a mirror of its underlying element. It simply presents the information in a simpler way.

## Expression

The following is what an epxression looks like:
```bdl
name : interface = Type<template>(parameters) [contracts]
```

Expressions instanciate and/or assign a value to a variable (`name`). Therefore they only hold values, they can instantiate a new value from a type
or copy an existing value.

*name*: This is the name of the instance. It is used to refer to this object locally or globally.
*interface*: This is the interface exposed by this name. In other word, when using this object, the user will only see the
interface and not the actual object.
*Type<template>*: The type of this object.
*contracts*: The contracts associated with this expression.
