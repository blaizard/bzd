load("@rules_cc//cc:defs.bzl", "cc_library")

TARGETS_ = [
    "linux",
    "linux_2_executors",
    "esp32_xtensa_lx6",
]

cc_library(
    name = "targets",
    visibility = ["//cc/bzd/platform:__pkg__"],
    deps = select({"//tools/bazel_build/platforms/target:auto": [":auto2"]} |
                  {name: ["//cc/targets/{}".format(name)] for name in TARGETS_}),
)

# Default targets when no constraint value is defined.

alias(
    name = "auto2",
    actual = select({
        "@platforms//os:linux": "//cc/targets/linux",
        "@//tools/bazel_build/platforms/isa:xtensa_lx6": "//cc/targets/esp32_xtensa_lx6",
    }),
)

alias(
    name = "auto",
    actual = select({
        "@platforms//os:linux": "//cc/targets/linux:auto",
        #"@//tools/bazel_build/platforms/isa:xtensa_lx6": "//cc/targets/esp32_xtensa_lx6",
    }),
    visibility = ["//visibility:public"],
)

# Create constraint values for all the C++ targets.

[constraint_value(
    name = name,
    constraint_setting = "//tools/bazel_build/platforms/target",
    visibility = ["//:__subpackages__"],
) for name in TARGETS_]
