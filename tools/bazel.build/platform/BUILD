package(default_visibility = ["//visibility:public"])

platform(
    name = "linux_x86_64",
    constraint_values = [
        "//tools/bazel.build/target/al:linux",
        "//tools/bazel.build/target/isa:x86_64",
    ],
)

platform(
    name = "linux_x86_64_clang",
    constraint_values = [
        "//tools/bazel.build/target/compiler:clang",
    ],
    parents = [":linux_x86_64"],
)

platform(
    name = "host_linux_x86_64_clang",
    host_platform = True,
    parents = [":linux_x86_64_clang"],
)
