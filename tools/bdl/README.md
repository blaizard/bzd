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

The symbol map object is created during the symbol resolution process. There is a ont to one relationship with a .bdl file and a symbol map.
It consists of the following:

- symbols: a symbols to element map. Elements in the symbol map contains all the information discovered after resolution.
- parsed: the raw parsed elements output of the file.

With the symbol map, one link symbols with their actual definition.

##### In-Memory Representation

#### Bdl Object

The bdl object stage creates a symbol map of the current compilation unit and serializes the output to a file (for re-use purpose).
This stage generates the preprocessed artifact that can be consumed by other compilation units importing other `bdl` files.

### Serialization

The preprocessed artifacts is serialized and written to disk if it aslo includes the resolution stage.

## Generators

The following describes built-in generators and how custom generators can be added.

### C++

The C++ formatter will generate a C++ compulation unit which interface is accessible from the `.h` counter part of the `.bdl` rule. For example, the exposed interface from `core.bdl` will be accessible by including `core.h`.
This is to ensure that inclusion can find the correspondiing header file.

Therefore to include `my/path/core.bdl` functionality in a C++ file, use the following:

```c++
#include "my/path/core.h"
```
