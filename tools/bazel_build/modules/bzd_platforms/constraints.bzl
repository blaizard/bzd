"""Constraints."""

AL_CONSTRAINTS = {
    "esp32": {
        "isa": ["xtensa_lx6"],
    },
    "esp32s3": {
        "isa": ["xtensa_lx7"],
    },
    "linux": {
        "alias": Label("@platforms//os:linux"),
        "isa": ["arm64", "x86_64"],
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
