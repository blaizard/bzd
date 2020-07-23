load("@debian_repository_rules//:debian.bzl", "debian_archive")

def load_linux_x86_64_python():
    debian_archive(
        name = "python3_linux_x86_64",
        build_file = "//toolchains/python/linux_x86_64:python.BUILD",
        urls = {
            "http://security.ubuntu.com/ubuntu/pool/main/p/python3.6/python3.6-minimal_3.6.9-1~18.04ubuntu1.1_amd64.deb": "802922343c443438bab007cb2fc7f8a7f059474d7f65aa867234cdf0a066e5c8",
            "http://security.ubuntu.com/ubuntu/pool/main/p/python3.6/libpython3.6-minimal_3.6.9-1~18.04ubuntu1.1_amd64.deb": "c2e7663624226088290e04560d97a36b9d49320809d8bd1280956cc52e1e27e1",
            "http://security.ubuntu.com/ubuntu/pool/main/p/python3.6/libpython3.6-stdlib_3.6.9-1~18.04ubuntu1.1_amd64.deb": "5f9804f17855f07e704ed98accf3e156a604b6ef1543b09fd552e8d385581638",
            "http://de.archive.ubuntu.com/ubuntu/pool/main/p/pyserial/python3-serial_3.4-2_all.deb": "3b16e77d7347717505b65bdd9dfb1d9ce71711494edf4fcbf46b62c7b075d137",
            "http://de.archive.ubuntu.com/ubuntu/pool/main/p/python3-stdlib-extensions/python3-distutils_3.6.9-1~18.04_all.deb": "f836b1e22923fa175c1968b9dbd916ae5639c9250e21bcfb88f9df2dc289f65c",
            "http://de.archive.ubuntu.com/ubuntu/pool/main/p/python3-stdlib-extensions/python3-lib2to3_3.6.9-1~18.04_all.deb": "3d20a1a4e5bfa53de66e06a293465855d113a633dac6fdeea3bd3f81f032e9fd",
        },
    )

    native.register_toolchains("//toolchains/python/linux_x86_64:toolchain")
