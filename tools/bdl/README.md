# BDL

BDL stands for Bzd Description Language and is the high level abstract language for this framework.

This document describes the tool behind the language, which role is to parse, validate and generate artifacts from bdl files.

## Overview

The process consists of the following:

```
Preprocess:
  [ parser ] - Parse the grammar from a bdl file and create a structured view.
  [ validator ] - Validates what can be validated at that point, this is to identify error at early level.
  [ build ] - Build the bdl object by creating a symbol map and resolving dependencies except for the composition elements.
  -> save bld object to the disk, this step is language agnostic.

The bdl file can be then used for:
[ File ] - file specific code generator.
[ Composition ] - Generate the composition step.
```

## Purpose

The code generation process has the following goals:

- Parse BDL files.
- Validate documents provided custom validators.
- Generate artifacts provided custom formatters.

## Stages

The process consists of 2 main stages, the preprocess and generation stages.

Both stages outpus artifacts, and the generation process consumes the preprocess artifact, this gives the possibility to cache and reuse output artifacts for various generation processes.

### Preprocess

#### Parser

The parser anaylizes a `bdl` file and extracts entities out of the file, decomposing the raw text into a visitable tree.
This is the only step in direct contact with the `bdl` file, it must therefore extract all useful information from this file.

Part of this process, the parser also goes through a basic validation step and a build step.

The build step hhas for responsibility to gather dependencies and resolve symbols if the dependency flag is set.

#### Symbol Map

A symbol map links symbols with their actual definition.

The symbol map object is created during the symbol resolution process.
It consists of a dictionary of fully qualified names to resolved elements. Elements that are unamed gets allocated a unique private fqn, that is visible only by this local compilation unit.

Each file contains all symbols including their dependencies except for unamed elements, or private symbols (starting with `_`).

#### Symbol Tree

##### In-Memory Representation

#### Bdl Object

The bdl object stage creates a symbol map of the current compilation unit and serializes the output to a file (for re-use purpose).
This stage generates the preprocessed artifact that can be consumed by other compilation units importing other `bdl` files.

### Serialization

The preprocessed artifacts is serialized and written to disk if it aslo includes the resolution stage.

## Generators

The following describes built-in generators and how custom generators can be added.

### C++

The C++ formatter will generate a C++ compilation unit which interface is accessible from the `.h` counter part of the `.bdl` rule. For example, the exposed interface from `core.bdl` will be accessible by including `core.h`.
This is to ensure that inclusion can find the correspondiing header file.

Therefore to include `my/path/core.bdl` functionality in a C++ file, use the following:

```c++
#include "my/path/core.hh"
```

#### Interfaces

Interfaces will generate an adapter class that implements the curiously recurring template pattern from which the component should inherits.

In addition, trivial types are generated to be accessible from the namespace declared in the `bdl` file.

For example, let's take the following interface, defined in the imaginary path "bzd/example/interface.bdl"

```bdl
namespace bzd.example;

interface MyInterface {
  enum Error {
    UNINITIALIZED,
    OTHER
  }
  method process() -> Error;
}
```

The implementation can access the followings:

```c++
#include "bzd/example/interface.hh" // Auto-generated from bdl.

class Implementation : public bzd::example::MyInterface<Implementation> // Adapter for the component.
{
public:
  constexpr Error process() noexcept // Implementation of the method.
  {
    return bzd::example::MyInterface::Error::OTHER; // Access trivial types.
  }
};
```

Interfaces defined in the BDL language are guaranteed to be accessible and callable from any node in the system.

#### External Interfaces

Sometimes, you want to create an interface that should be defined in the C++ language itself for example, in order
to benefit from some features that are not available in the BDL language.

Note, doing so, limits the interface to the composition only. Such interface cannot be accessed from the overall system.

To define such interface, simply do the followng:
```bdl
namespace bzd.example;

extern interface MyInterface;
```

The user is then tasked to provide the C++ symbol within the namespace. Note the interface must use the CRTP pattern.
Here is an example of what could be such interface:
```c++
template <class Impl>
class MyInterface
{
public:
  constexpr Error process() noexcept // Implementation of the method.
  {
    // getMethod is a helper that ensures the signature of the interface matches the implementation
    // and that returns the implementation object.
    return bzd::impl::getMethod(this, &MyInterface::process, &Impl::process)->process();
  }
};
```

Alternatively virtual interfaces can also be used as follow:
```c++
class MyInterface
{
public:
  virtual Error process() noexcept = 0;
};
```

#### Interfaces Casting

Object which implements an interface are casted down to their interfaces when used in the composition, this is done
automatically by the code generator and it ensures that the object is used by its interface only.

The code generation specialize the `bzd::Interface` type with the fully qualified name of the object type. It can then
be used like this:
```c++
bzd::Interface<"bzd.example.MyInterface">::cast(object);
```
