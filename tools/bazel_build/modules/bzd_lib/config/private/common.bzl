"""Common utilities for config."""

ConfigInfo = provider(
    doc = "Provider of the configuration.",
    fields = {
        "data": "data associated with this configuration.",
        "internal": "Internal representation of the configuration.",
        "runfiles": "runfiles associated with this configuration.",
    },
)
