# Build

Resolving symbols within a `bdl` file is done at different build stages to allow lazy composition.

## Overview

The build process consists of 3 main stages, the `preprocess` and the `composition` stages.

Both stages outpus artifacts, and the `composition` process consumes the `preprocess` artifact, this gives the possibility to cache and reuse outputs for various composition processes.

## Preprocess

### Parser

The parser anaylizes a `bdl` file and extracts entities out of the file, decomposing the raw text into a visitable tree.
This is the only step in direct contact with the `bdl` file, it must therefore extract all useful information from this file.

Part of this process, the parser also goes through a basic validation step and a build step.

The build step has for responsibility to gather dependencies and resolve symbols if the dependency flag is set.

### Bdl Object

The output of the preprocess stage is a `bdl` object which mainly consist of a `Symbol Map` and a `Symbol Tree`.

A `Symbol Map` links symbols with their actual definition. It consists of a dictionary of fully qualified names to resolved elements. Elements that are unamed gets allocated a unique private fqn, that is visible only by this local compilation unit.

A `Symbol Tree` is a hierachical view of the file, it can be uised to recreate a given file and should contains all the necessary
information. It only uses symbols defined in the `Symbol Map`. In short it is a hierachical view of a `Symbol Map`.

A bdl file is serialized into a `.o` file for caching purpose. Each `.bdl` should have its corresponding `.o` file after the build process.

### Symbol Resolution

During file `preprocessing` stage, all symbols are resolved and are matched with the previously discovered symbols.
These symbols are stored into the `Symbol Map`.
In other word, all symbols used must have been precendently defined. However, there are few exception that does
not follow this rule, which are any symbol within a `composition` scope or within a `config` section.

In a `composition` scope, only the name of the entity is registered, but the entity itself is not resolved, in other
word, in `hello = my.fund(a = 1);`, the name `hello` is registed but the function `my.func` is not resolved, so it can
be declared later on.

In a `config` section, nothing is resolved, this is because config names are not part of the id tree, they are virtual
entities that do not concretly translate into code. Parameters in `config` are only resolve when the corresponding
entity is instanciated.

## Composition

The composition stage brings all components together to compose a running system.

Components might come from the platform used, others are functional to perform tasks related to the application.

During `composition` stage, all top level composition elements are being resolved. Nested composition elements are
only resolved if the corresponding component is instantiated. Unused components in the build tree are automatically removed.

The composition stage is done in 2 sub-stages, the first one that only process certain type of builtin expression, such as `bind`
for example. This assigns contracts to expressions before being evaluated.
The second stage process all the rest of top level elements and their dependencies.

## Generators

Generators can follow a `preprocess` or a `composition` stage and are responsible for generating additional output.
For example, they are used to re-generate `bdl` files after being preprocessed or generate C++ code after the composition stage.
