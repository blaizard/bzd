filegroup(
    name = "header_files",
    srcs = glob([
        "include/c++/v1/**/*",
        "include/x86_64-unknown-linux-gnu/c++/v1/**/*",
        "lib/clang/15.0.6/include/**/*",
    ])
)

# Create a package with only what is needed
genrule(
    name = "package",
    srcs = [
        "@linux_x86_64_clang//:all_files",
        ":clang_format",
        ":clang_tidy",
        ":llvm_symbolizer",
    ],
    outs = ["linux_x86_64_16.0.6.tar.xz"],
    tags = ["manual"],
    cmd = """
    tar -cJf $@ --dereference --transform 's/^\\./linux_x86_64_16.0.6/' -C external/linux_x86_64_clang_16_0_6 .
    """
)
