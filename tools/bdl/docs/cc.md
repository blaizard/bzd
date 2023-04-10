# C++

The C++ formatter will generate a C++ compilation unit which interface is accessible from the `.h` counter part of the `.bdl` rule. For example, the exposed interface from `core.bdl` will be accessible by including `core.h`.
This is to ensure that inclusion can find the correspondiing header file.

Therefore to include `my/path/core.bdl` functionality in a C++ file, use the following:

```c++
#include "my/path/core.hh"
```

## Interfaces

Interfaces will generate an adapter class that implements the curiously recurring template pattern from which the component should inherits.

In addition, trivial types are generated to be accessible from the namespace declared in the `bdl` file.

For example, let's take the following interface, defined in the imaginary path "bzd/example/interface.bdl"

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

The implementation can access the followings:

```c++
#include "bzd/example/interface.hh" // Auto-generated from bdl.

class Implementation : public bzd::example::MyInterface<Implementation> // Adapter for the component.
{
public:
  constexpr Error process() noexcept // Implementation of the method.
  {
    return bzd::example::MyInterface::Error::other; // Access trivial types.
  }
};
```

Interfaces defined in the BDL language are guaranteed to be accessible and callable from any node in the system.

### External

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
    // getImplementation is a helper that ensures the signature of the interface matches the implementation
    // and that returns the implementation object.
    return bzd::impl::getImplementation(this, &MyInterface::process, &Impl::process)->process();
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

### Casting

Object which implements an interface are casted down to their interfaces when used in the composition, this is done
automatically by the code generator and it ensures that the object is used through its interface only.

The code generation specialize the `bzd::Interface` type with the fully qualified name of the object type. It can then
be used like this:

```c++
bzd::Interface<"bzd.example.MyInterface">::cast(object);
```

## Composition

When composing component together, we need to distinguish between several types of component.

1. `platform` components, which are target specific and might or might not be singleton on a target.
  - For example: a proactor, an stdout...
2. Functional components with no workloads that are the input of 2 other components running on different executors.
  - It can be part of the registry of both executors or only of one.
3. Functional components with a workload.
  - These must be assigned to an executor.

Should we have a single registry? or a common registry and an executor specific registry?
This could be solved as an implementation detail by the component itself, using a singleton pattern for example.

Ultimately `1.` and `2.` should be the same.

Use case, stdout backend running on a specific executor:
```bdl
component Stdout {
interface:
  method backend();
composition:
  this.backend();
}

composition {
  exec1 = Executor() [executor];
  exec2 = Executor() [executor];
  stdout = Stdout() [executor(exec1)];
}
```
<- This is not a good composition. It should be splitted into a backend and an 2 frontends.
They would both communicate 

Use case:
