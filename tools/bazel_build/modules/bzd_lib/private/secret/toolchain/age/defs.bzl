"""Make age tools available."""

load("//:defs.bzl", "bzd_http_archive", "bzd_repository_multiplatform_maker")

def age_install():
    """Install the age and age-keygen tool.

    Make them available at @age//:age and @age//:age-keygen
    """
    bzd_repository_multiplatform_maker(
        name = "age",
        repositories = [{
            "build_file": Label("//private/secret/toolchain/age:age.BUILD.bazel"),
            "compatible_with": [
                Label("@bzd_platforms//al_isa:linux-x86_64"),
            ],
            "http": [
                bzd_http_archive(
                    integrity = "sha256-vcacCcvdbPix8zPTcqH1gkezozFGQGMz4wwPJuj1E3c=",
                    url = "https://github.com/FiloSottile/age/releases/download/v1.3.1/age-v1.3.1-linux-amd64.tar.gz",
                    strip_prefix = "age",
                ),
            ],
        }],
        expose = {
            "age": "age",
            "age-keygen": "age-keygen",
        },
    )
