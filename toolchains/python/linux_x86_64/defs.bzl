load("@debian_repository_rules//:debian.bzl", "debian_archive")

def load_linux_x86_64_python():
    debian_archive(
        name = "python3_linux_x86_64",
        build_file = "//toolchains/python/linux_x86_64:python.BUILD",
        urls = {
            "http://security.ubuntu.com/ubuntu/pool/main/p/python3.8/python3.8-minimal_3.8.10-0ubuntu1~20.04_amd64.deb": "80854d4ad73d53614fa50d90cfb035130a661fb203f4816c68f8abde4c4cbdc0",
            "http://security.ubuntu.com/ubuntu/pool/main/p/python3.8/libpython3.8-minimal_3.8.10-0ubuntu1~20.04_amd64.deb": "1ab56862e653fc52315c79422ed17af7191ca888ce97dd220c04a8243c4b8de0",
            "http://security.ubuntu.com/ubuntu/pool/main/p/python3.8/libpython3.8-stdlib_3.8.10-0ubuntu1~20.04_amd64.deb": "0e7da3725001d73c55da863307d52e6ca5dfe23bb56f6c505e2472d6bdc22ca2",
            #"http://archive.ubuntu.com/ubuntu/pool/main/p/pyserial/python3-serial_3.4-2_all.deb": "3b16e77d7347717505b65bdd9dfb1d9ce71711494edf4fcbf46b62c7b075d137",
            #"http://archive.ubuntu.com/ubuntu/pool/main/p/python3-stdlib-extensions/python3-distutils_3.6.5-3_all.deb": "29fb33ae1edb6da2cadb95506b0b99127d8b8405b2eb9f3e78dfaa4109fbc677",
            "http://archive.ubuntu.com/ubuntu/pool/main/p/python3-stdlib-extensions/python3-lib2to3_3.8.10-0ubuntu1~20.04_all.deb": "d25b7a114ab08e6f318d96ae8d71c3c24c3b9eaceb84cb1abf476979875521ab",
        },
    )

    native.register_toolchains("//toolchains/python/linux_x86_64:toolchain")
