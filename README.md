# ::bzd

[![Build Status](https://travis-ci.org/blaizard/cpp-async.svg?branch=master)](https://travis-ci.org/blaizard/cpp-async)
[![CI](https://github.com/blaizard/cpp-async/workflows/CI/badge.svg)](https://github.com/blaizard/cpp-async/actions)

Fullstack, lightweight and flexible framework for IoT applications.
This mono-repository enables the following technologies:

|C++14                         |Python3                          |Node (v13+)                    |Vue                           |Bazel                           |
|:----------------------------:|:-------------------------------:|:-----------------------------:|:----------------------------:|:------------------------------:|
|![](./docs/assets/png/cpp.png)|![](./docs/assets/png/python.png)|![](./docs/assets/png/node.png)|![](./docs/assets/png/vue.png)|![](./docs/assets/png/bazel.png)|

## Menu

- [Bazel Rules](./docs/bazel_rules.md)
- [Manifest](./docs/manifest.md)
- [API](./docs/api/cc/index.md)

## Overview

This framework is developped to run on all sort of embedded platforms, from 8-bit bare bones MCU to high performance compute unit.
It is made to remove any dependencies from STL, but prtempDirPathovide similar set of containers and utilities to develop applications in an
efficient and safe manner.

It features the followings:
- No dynamic allocations. All storage is declared either at compile time or on the stack.
- Self contained, no reliance on third-party nor STL dependencies.
- Flexible error handling model.
- Compile-time string formting tool.
- Lightweight.
- Usefull set of containers.
- API documentation ([see here](./docs/api/cc/index.md))

## Memory footprint

All classes offered by this library have the same objective to have a fixed memory size. Their footprint or maximum size
is determined at compile time, ensuring no memory allocation at runtime, making this library ideal for any safety-critical application.

Notions:
- Task. A task is similar to the concept of coroutine in C++20, except that there it ensures no dynamic allocation. Tasks are made to run from low ressources embedded systems to high scale compute units.
