# BDL

BDL stands for Blaizard Description Language and is the high level abstract language for this framework.

This document describes the tool behind the language, which role is to parse, validate and generate artifacts from bdl files.

## Overview

The process consists of the following:

```

[                   Preprocess              ] -> [                        Generation                       ]

[ parser ] -> [ validator ] -> [ Bdl Object ] -> [ symbol resolution ] -> [ generator ] and/or [ validator ]
                                    |                                         |
                                  output                                    output
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

#### Bdl Object

The bdl object stage creates a symbol map of the current compilation unit and serializes the output to a file (for re-use purpose).
This stage generates the preprocessed artifact that can be consumed by other compilation units importing other `bdl` files.

### Serialization

The preprocessed artifacts is serialized and written to disk if and only if, it aslo includes dependencies.

## Generators

The following describes built-in generators and how custom generators can be added.

### C++

The C++ formatter will generate a C++ compulation unit which interface is accessible from the `.h` counter part of the `.bdl` rule. For example, the exposed interface from `core.bdl` will be accessible by including `core.h`.
This is to ensure that inclusion can find the correspondiing header file.

Therefore to include `my/path/core.bdl` functionality in a C++ file, use the following:

```c++
#include "my/path/core.h"
```
