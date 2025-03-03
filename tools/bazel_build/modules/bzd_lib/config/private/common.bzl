"""Common utilities for config."""

ConfigInfo = provider(
    doc = "Provider of the configuration.",
    fields = {
        "data": "data associated with this configuration.",
        "json": "Configuration in JSON format.",
        "runfiles": "runfiles associated with this configuration.",
    },
)
