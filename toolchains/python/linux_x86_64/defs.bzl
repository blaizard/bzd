load("@debian_repository_rules//:debian.bzl", "debian_archive")

def load_linux_x86_64_python():
    debian_archive(
        name = "python3_linux_x86_64",
        build_file = "//toolchains/python/linux_x86_64:python.BUILD",
        urls = {
            "http://security.ubuntu.com/ubuntu/pool/main/p/python3.6/python3.6-minimal_3.6.9-1~18.04ubuntu1_amd64.deb": "6ea7828a83de9ee539b491527045d2a0a66d4e105528325bbe43917fb635da14",
            "http://security.ubuntu.com/ubuntu/pool/main/p/python3.6/libpython3.6-minimal_3.6.9-1~18.04ubuntu1_amd64.deb": "d12e9823bfc6fd96d9545b13950837ad7fd58bc07919b4d28d9a481f0d51c930",
            "http://security.ubuntu.com/ubuntu/pool/main/p/python3.6/libpython3.6-stdlib_3.6.9-1~18.04ubuntu1_amd64.deb": "18dc6c871a8e9cd3dcca9aaade7d1874f73180ace561a47542cf3cbc3afa882f",
            "http://de.archive.ubuntu.com/ubuntu/pool/main/p/pyserial/python3-serial_3.4-2_all.deb": "3b16e77d7347717505b65bdd9dfb1d9ce71711494edf4fcbf46b62c7b075d137",
            "http://de.archive.ubuntu.com/ubuntu/pool/main/p/python3-stdlib-extensions/python3-distutils_3.6.9-1~18.04_all.deb": "f836b1e22923fa175c1968b9dbd916ae5639c9250e21bcfb88f9df2dc289f65c",
        },
    )

    native.register_toolchains("//toolchains/python/linux_x86_64:toolchain")
