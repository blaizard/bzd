"""BDL providers."""

visibility(["//..."])

BdlInfo = provider(
    doc = "Provider for bdl files.",
    fields = {
        "direct": "The preprocessed bdl files directly associated with this rule.",
        "files": "All files, contains the same set of files as in `sources`, but uses a different format for convenience.",
        "search_paths": "Ordered list of directory paths to search for preprocessed object files.",
        "sources": "Pair of input and preprocessed bdl files.",
    },
)

BdlTargetInfo = provider(
    doc = "Provider for a target.",
    fields = {
        "binary": "Binary target.",
        "composition": "List of composition files for this target.",
        "data": "Dictionary of files or bazel targets associated with target.",
        "deps": "List of dependencies for this target.",
        "language": "Language used for this target.",
    },
)

BdlCompositionInfo = provider(
    doc = "Provider for the system composition.",
    fields = {
        "bdls": "All bdls info providers for the system.",
        "deps": "Target-specific dependencies.",
    },
)

BdlSystemJsonInfo = provider(
    doc = "Provider for a system json representation of a system.",
    fields = {
        "json": "Dictionary of json files representing the system, keyed by target.",
    },
)
