# bzd

Lightweight and flexible framework for cooperative and event-base programming.
Requires any compiler with support for C++14 (at least).

This framework is developped to run on all sort of embedded platforms, from 8-bit bare bones MCU to high performance compute unit.
It is made to remove any dependencies from STL, but provide similar set of containers and utilities to develop applications in an
efficient and safe manner.

It features the followings:
- No dynamic allocations. All storage is declared either at compile time or on the stack.
- Self contained, no reliance on third-party nor STL dependencies.
- Flexible error handling model.
- Compile-time string formting tool.
- Lightweight.
- Usefull set of containers.
- API documentation ([see here](./docs/api/index.md))

## Memory footprint

All classes offered by this library have the same objective to have a fixed memory size. Their footprint or maximum size
is determined at compile time, ensuring no memory allocation at runtime, making this library ideal for any safety-critical application.

Notions:
- Task. A task is similar to the concept of coroutine in C++20, except that there it ensures no dynamic allocation. Tasks are made to run from low ressources embedded systems to high scale compute units.
