"""Providers for tscircuit."""

BzdTscircuitInfo = provider(
    doc = "Source files of a tscircuit library.",
    fields = {
        "files": "Depset of the source files.",
    },
)
