"""Runner providers."""

BzdRunnerOptionsInfo = provider(
    doc = "Options passed to a runner through environment variables.",
    fields = {
        "files": "Dictionary of environment variable names to files.",
        "values": "Dictionary of environment variable names to values.",
    },
)
