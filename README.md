# cpp-bzd

Lightweight and flexible framework for cooperative and event-base programming.
This framework aims at safety and prevents any dynamically allocation and exception to ensure full control the memory management.

It requires C++14.

Notions:
- Task. A task is a similar to the concept of coroutine in C++20, except that there is no dynamic allocation. Tasks are made to run from low ressources embedded systems to high scale compute units.
