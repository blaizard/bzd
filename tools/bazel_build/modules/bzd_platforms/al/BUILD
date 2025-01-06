load("@bazel_skylib//lib:selects.bzl", "selects")
load("//:constraints.bzl", "AL_CONSTRAINTS")

package(default_visibility = ["//visibility:public"])

alias(
    name = "al",
    actual = "@platforms//os",
)

[alias(
    name = al,
    actual = data["alias"],
) for al, data in AL_CONSTRAINTS.items() if data.get("alias") != None]

[selects.config_setting_group(
    name = al,
    match_any = data["children"],
) for al, data in AL_CONSTRAINTS.items() if data.get("children") != None]

[constraint_value(
    name = al,
    constraint_setting = ":al",
) for al, data in AL_CONSTRAINTS.items() if data.get("alias") == None and data.get("children") == None]
