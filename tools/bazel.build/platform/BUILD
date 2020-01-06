package(default_visibility = ["//visibility:public"])

platform(
    name = "linux_x86_64",
    constraint_values = [
        "//tools/bazel.build/target:linux",
        "//tools/bazel.build/target:x86_64",
        "@platforms//os:linux",
        "@platforms//cpu:x86_64",
    ],
)

platform(
    name = "linux_x86_64_clang",
    constraint_values = [
        "//tools/bazel.build/target:clang",
    ],
    parents = [":linux_x86_64"],
)

platform(
    name = "host_linux_x86_64_clang",
    host_platform = True,
    parents = [":linux_x86_64_clang"],
)
