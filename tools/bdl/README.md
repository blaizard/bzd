# BDL

BDL stands for Blaizard Description Language and is the high level abstract language for this framework.

This document describes the tool behind the language, which role is to parse, validate and generate artifacts from bdl files.

## Overview

The process consists of the following:

```
[ parser ] -> [ syntax validator ] -> [ Bdl Object ] -> [ symbol resolution ] -> [ generator ] and/or [ validator ]
                                            |                                         |
                                        *.o files                              *.bdl / *.h / ...
```

## Purpose

The code generation process has the following goals:

- Parse BDL files.
- Validate documents provided custom validators.
- Generate artifacts provided custom formatters.

## Process

### Parser

The parser anaylizes a `bdl` file and extracts entities out of the file, decomposing the raw text into a visitable tree.
This is the only step in direct contact with the `bdl` file, it must therefore extract all useful information from this file.

Part of this process, the parser also goes through a basic validation step, to catch potential issues at the syntax level.

### Bdl Object

The bdl object stage creates a symbol map of the current compilation unit and serializes the output to a file (for re-use purpose).
This stage generates the preprocessed artifact that can be consumed by other compilation units importing other `bdl` files.
