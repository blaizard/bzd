"""JSON extension for bdl rules."""

load("//private/extensions/json:composition.bzl", "generator_json_composition")

extension = {
    "composition": {
        "generator": generator_json_composition,
    },
}
