# Bzd Platforms

Platforms are a set of constraints. Their string representation is the constraints stringified separated by a dash `-`.

A compiler platform is typically defined as a pair of 2 constraints: `<AL>-<ISA>`.

# Abstraction Layer (AL)

This constraints cover the abstraction layer with the hardware, such layer can be `linux` or `esp32` for example.

This should cover the hardware and the SDK.

# Instruction Set Architecture (ISA)

This constraints cover the exact CPU, and more precisely the instruction set architecture.

This shall cover every details and specificity of the compute unit, such as hardware accelerators, etc.

Examples are `x86_64`, ...
