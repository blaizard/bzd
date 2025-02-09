"""BDL providers."""

visibility(["//..."])

BdlTagInfo = provider(
    doc = "Empty provider to identify a BDL library.",
    fields = [],
)

BdlInfo = provider(
    doc = "Provider for bdl files.",
    fields = {
        "files": "All files, contains the same set of files as in `sources`, but uses a different format for convenience.",
        "search_formats": "Set of search formats string to locate the preprocessed object files.",
        "sources": "Pair of input and preprocessed bdl files.",
    },
)

BdlTargetInfo = provider(
    doc = "Provider for a target.",
    fields = {
        "binary": "Binary target.",
        "composition": "List of composition files for this target.",
        "deps": "List of dependencies for this target.",
        "language": "Language used for this target.",
    },
)

BdlSystemInfo = provider(
    doc = "Provider for a system rule.",
    fields = {
        "data": "Data specific keyed by extension.",
    },
)

BdlSystemJsonInfo = provider(
    doc = "Provider for a system json representation of a system.",
    fields = {
        "json": "Dictionary of json files representing the system, keyed by target.",
    },
)
