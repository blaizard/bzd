"""Common utilities for config."""

ConfigInfo = provider(
    doc = "Provider of the configuration.",
    fields = {
        "json": "Configuration in JSON format.",
        "runfiles": "runfiles associated with this configuration.",
    },
)

ConfigOverrideInfo = provider(
    doc = "Information about override",
    fields = {
        "files": "Dictionary of location and config value overrides.",
        "runfiles": "Dictionary of location and runfiles.",
    },
)

def label_to_key(label):
    """Convert a label into a dotted key."""

    return (label.repo_name + "." + ".".join(label.package.split("/")) + "." + label.name).strip(".")

# Number of possible overrides for a target to be used.
# This is a workaround until we have string_list_flag: https://github.com/bazelbuild/bazel/issues/17828
NB_OVERRIDE_TARGETS = 10
override_targets = ["//config:override" + str(i) for i in range(0, NB_OVERRIDE_TARGETS)]
