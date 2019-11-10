# cpp-bzd

Lightweight and flexible framework for cooperative and event-base programming.

This framework is developped to run on all kind of embedded platforms, from 8-bit bare bones MCU to high performance compute unit.

It features the followings:
- No dynamic allocations.
- No exceptions ensure.
- Compile-time string formting tool.
- Lightweight.
- Usefull set of containers.

It requires C++14.

Notions:
- Task. A task is a similar to the concept of coroutine in C++20, except that there is no dynamic allocation. Tasks are made to run from low ressources embedded systems to high scale compute units.
