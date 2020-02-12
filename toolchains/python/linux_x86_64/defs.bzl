load("@debian_repository_rules//:debian.bzl", "debian_archive")

def load_linux_x86_64_python():
    debian_archive(
        name = "python3_linux_x86_64",
        build_file_content = """py_runtime(
        name = "runtime",
        files = glob(["usr/lib/**/*"]),
        interpreter = "usr/bin/python3.6",
        python_version = "PY3",
        visibility = ["//visibility:public"],
    )""",
        urls = {
            "http://security.ubuntu.com/ubuntu/pool/main/p/python3.6/python3.6-minimal_3.6.8-1~18.04.3_amd64.deb": "5f1ce00fab73a47dd1144c7979a71a8b87ec51a6fe227933a2a4d11f81889d2b",
            "http://security.ubuntu.com/ubuntu/pool/main/p/python3.6/libpython3.6-minimal_3.6.8-1~18.04.3_amd64.deb": "b678aa92ba35b26e1ead5a59a29b7f7dc9da9063f4216b3d40990417659ce50d",
            "http://security.ubuntu.com/ubuntu/pool/main/p/python3.6/libpython3.6-stdlib_3.6.8-1~18.04.3_amd64.deb": "5acb9bfd339329d15f2b6cd6436742c914c1e8aee0d7359bbf2c1dc3c4de5c04",
            "http://de.archive.ubuntu.com/ubuntu/pool/main/p/pyserial/python3-serial_3.4-2_all.deb": "3b16e77d7347717505b65bdd9dfb1d9ce71711494edf4fcbf46b62c7b075d137",
        },
    )

    native.register_toolchains("//toolchains/python/linux_x86_64:toolchain")
