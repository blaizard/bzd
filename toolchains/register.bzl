load("//toolchains/nodejs/linux_x86_64_yarn:defs.bzl", "load_linux_x86_64_yarn")

def toolchains_register():
    load_linux_x86_64_yarn()

