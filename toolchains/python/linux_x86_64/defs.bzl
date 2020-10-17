load("@debian_repository_rules//:debian.bzl", "debian_archive")

def load_linux_x86_64_python():
    debian_archive(
        name = "python3_linux_x86_64",
        build_file = "//toolchains/python/linux_x86_64:python.BUILD",
        urls = {
            "http://security.ubuntu.com/ubuntu/pool/main/p/python3.6/python3.6-minimal_3.6.5-3_amd64.deb": "a033297a970487552f82a210df18b206ddc465627e68ddcbad2603818083d946",
            "http://security.ubuntu.com/ubuntu/pool/main/p/python3.6/libpython3.6-minimal_3.6.5-3_amd64.deb": "ce512b47c542886ab93a2667f6ea90edfa063eb0bc5d6a9ac782dfa44e201be2",
            "http://security.ubuntu.com/ubuntu/pool/main/p/python3.6/libpython3.6-stdlib_3.6.5-3_amd64.deb": "ff164729742f8e8cfc1024d6b62d9d7dff64761a0a83ed6028e44dc6dfa6a618",
            "http://de.archive.ubuntu.com/ubuntu/pool/main/p/pyserial/python3-serial_3.4-2_all.deb": "3b16e77d7347717505b65bdd9dfb1d9ce71711494edf4fcbf46b62c7b075d137",
            "http://de.archive.ubuntu.com/ubuntu/pool/main/p/python3-stdlib-extensions/python3-distutils_3.6.5-3_all.deb": "29fb33ae1edb6da2cadb95506b0b99127d8b8405b2eb9f3e78dfaa4109fbc677",
            "http://de.archive.ubuntu.com/ubuntu/pool/main/p/python3-stdlib-extensions/python3-lib2to3_3.6.5-3_all.deb": "da61799a4268583ec7ef6cda2230e6b9df27d4455783f9fa708900c4ca02179a",
        },
    )

    native.register_toolchains("//toolchains/python/linux_x86_64:toolchain")
