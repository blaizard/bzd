"""Common utilities for config."""

ConfigInfo = provider(
    doc = "Provider of the configuration.",
    fields = {
        "data": "Data associated with this configuration.",
        "internal": "Internal representation of the configuration.",
        "runfiles": "Runfiles associated with this configuration.",
    },
)

ConfigSourceInfo = provider(
    doc = "Provider for a type to be used as source.",
    fields = {
        "content": "The content to be associated with this configuration.",
        "file": "The file to be associated with this configuration.",
        "metadata": "Metadata to be linked with this configuration.",
    },
)
