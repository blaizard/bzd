# tscircuit

Rules to run [tscircuit](https://github.com/tscircuit/tscircuit) circuits with the
[tscircuit CLI](https://github.com/tscircuit/cli).

```python
load("@bzd_tscircuit//:defs.bzl", "bzd_tscircuit_binary")

bzd_tscircuit_binary(
    name = "board",
    srcs = ["board.circuit.tsx"],
    main = "board.circuit.tsx",
    deps = [":components"],  # bzd_tscircuit_library
)
```

`bzd_tscircuit_binary` creates three targets:

- `//path/to:board`: starts the development server, to be used with `bazel run`.
- `//path/to:board.svg`: generates the schematic, the output path is the first argument.
- `//path/to:board.json`: generates the circuit JSON, the output path is the first argument.
