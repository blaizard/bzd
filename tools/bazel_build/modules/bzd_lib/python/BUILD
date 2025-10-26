load("@rules_python//python:pip.bzl", "compile_pip_requirements")

# bazel run @bzd_lib//python:requirements.update
compile_pip_requirements(
    name = "requirements",
    timeout = "long",
    generate_hashes = False,
    requirements_in = ":requirements.in",
    requirements_txt = ":requirements.txt",
    tags = ["manual"],
)
