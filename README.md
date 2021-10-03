# ::bzd

[![Build Status](https://travis-ci.com/blaizard/cpp-async.svg?branch=master)](https://travis-ci.com/blaizard/cpp-async)
[![CI](https://github.com/blaizard/cpp-async/workflows/CI/badge.svg)](https://github.com/blaizard/cpp-async/actions)

Fullstack, lightweight and flexible framework for IoT applications.
This mono-repository enables the following technologies:

|      C++17 (+ coroutines)      |             Python3.8             |           Node (v13+)           |              Vue               |              Bazel               |              Eagle               |
| :----------------------------: | :-------------------------------: | :-----------------------------: | :----------------------------: | :------------------------------: | :------------------------------: |
| ![](./docs/assets/png/cpp.png) | ![](./docs/assets/png/python.png) | ![](./docs/assets/png/node.png) | ![](./docs/assets/png/vue.png) | ![](./docs/assets/png/bazel.png) | ![](./docs/assets/png/eagle.png) |

## Overview

This framework is built to run on all sort of embedded platforms, from bare metals 8-bit microcontrollers to high performance compute units.
It is made to remove any dependencies from STL, but provide a similar set of containers and utilities to develop applications in an
efficient and safe manner.

It features the followings:

- No dynamic allocations. All storage is declared either at compile time or on the stack.
- Self contained, no reliance on third-party nor STL dependencies.
- Flexible error handling model.
- Compile-time string formating tool.
- Lightweight.
- Usefull set of containers.

## C++ Platforms

The following c++ target platforms are currently supported:

| Name                 | Compiler | Version |
| -------------------- | -------- | ------- |
| linux_x86_64_clang   | clang    | 12.0.0  |
| linux_x86_64_gcc     | gcc      | 10.2.0  |
| esp32_xtensa_lx6_gcc | gcc      | 10.2.0  |

## Code Quality

All supported languages come with their set of code quality related tools.
To run all static sanity checks at once, code formatters and documentation generators, run the following command:

```
./sanitize.sh
```

To run all tests, run the following command:

```
./quality_gate.sh
```

This is a non-exhaustive list of languages and their supported tools. In addition to that
formater and linters for YAML, json and markdown are also available.
|Language|Formatters|Code Analyzers|Code Coverage|Test Framework|Documentation|
|--|--|--|--|--|--|
|C++|clang-format|asan, lsan|Yes|Yes|doxygen-based|
|Python|yapf|mypy|Yes|Yes||
|Node|prettier|eslint|Yes|Yes||
|Skylark|buildifier|buildifier|||stardoc|

## Memory footprint

All classes offered by this library have the same objective to have a fixed memory size. Their footprint or maximum size
is determined at compile time, ensuring no memory allocation at runtime, making this library ideal for any safety-critical application.
