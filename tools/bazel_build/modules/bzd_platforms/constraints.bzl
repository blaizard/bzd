"""Constraints."""

AL_CONSTRAINTS = {
    "esp32": {},
    "esp32s3": {},
    "linux": {
        "alias": Label("@platforms//os:linux"),
    },
}

ISA_CONSTRAINTS = {
    "arm64": {
        "alias": Label("@platforms//cpu:arm64"),
        "synonmys": ("aarch64",),
    },
    "x86_64": {
        "alias": Label("@platforms//cpu:x86_64"),
        "synonyms": ("x86-64", "x64", "amd64"),
    },
    "xtensa_lx6": {},
    "xtensa_lx7": {},
}
