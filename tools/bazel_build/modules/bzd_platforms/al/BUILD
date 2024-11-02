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

[constraint_value(
    name = al,
    constraint_setting = ":al",
) for al, data in AL_CONSTRAINTS.items() if data.get("alias") == None]
