"""Common utilities for config."""

ConfigInfo = provider(
    doc = "Provider of the configuration.",
    fields = {
        "json": "Configuration in JSON format.",
        "runfiles": "runfiles associated with this configuration.",
    },
)
