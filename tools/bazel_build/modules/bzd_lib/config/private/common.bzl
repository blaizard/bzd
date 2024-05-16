"""Common utilities for config."""

ConfigInfo = provider(
    doc = "Provider of the configuration.",
    fields = {
        "json": "Configuration in JSON format.",
    },
)

ConfigOverrideInfo = provider(
    doc = "Information about override",
    fields = {
        "files": "Dictionary of location and overrides.",
    },
)

def label_to_key(label):
    """Convert a label into a dotted key."""

    return (label.repo_name + "." + ".".join(label.package.split("/")) + "." + label.name).strip(".")
