load("@bzd_lib//:defs.bzl", "bzd_diff_test", "bzd_executable_test")
load("@rules_shell//shell:sh_binary.bzl", "sh_binary")
load("//private:bundle_binary.bzl", "bzd_bundle_binary")
load("//private:bundle_tar.bzl", "bzd_bundle_tar")

sh_binary(
    name = "dummy",
    srcs = [
        "dummy.sh",
    ],
    data = [
        "dummy.txt",
    ],
)

bzd_bundle_tar(
    name = "empty_tar",
    compression = None,
    output = "empty_tar.tar",
)

bzd_diff_test(
    name = "reproducible_empty_tar",
    file1 = ":empty_tar.tar",
    file2 = "expected_empty.tar",
)

bzd_bundle_tar(
    name = "generate_tar",
    compression = None,
    executables = {
        "a": ":dummy",
    },
    output = "generate_tar.tar",
)

bzd_diff_test(
    name = "reproducible_tar",
    file1 = ":generate_tar.tar",
    file2 = "expected.tar",
)

bzd_bundle_tar(
    name = "generate_tar_gz",
    compression = "gz",
    executables = {
        "a": ":dummy",
    },
    output = "generate_tar_gz.tar.gz",
)

bzd_diff_test(
    name = "reproducible_tar_gz",
    file1 = ":generate_tar_gz.tar.gz",
    file2 = "expected.tar.gz",
)

bzd_bundle_binary(
    name = "binary",
    executable = ":dummy",
)

bzd_executable_test(
    name = "test_binary",
    executable = ":binary",
    expected_output = "from dummy.sh\nfrom dummy.txt",
    expected_returncode = "0",
)

bzd_diff_test(
    name = "reproducible_binary",
    file1 = ":binary",
    file2 = "expected_binary",
)
