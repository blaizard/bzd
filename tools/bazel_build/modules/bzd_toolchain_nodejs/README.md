# TODO

- Fix pnpm store-dir to use a specific or configurable path.
- Remove tools/bazel_build/modules/bzd_toolchain_nodejs/nodejs_web/defs.old.bzl
- Remove tools/data attributes of nodejs_install?
- Add docker support

## Absolute paths

All assets and sources files can be accessed by their absolute path using the alias `#bzd`.

In some cases, with `nodejs_web`, this is not possible, for example when referencing a path in SCSS with the `url` attribute.
In that case, you must prepend the path with `@`.
