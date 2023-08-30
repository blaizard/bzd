# NodeJs / NodeJsWeb

## TODO

- Fix pnpm store-dir to use a specific or configurable path.
- Remove tools/data attributes of nodejs_install?
- Add docker support
- Handle dev/prod
- Support coverage
- Remove those errors:
  (18:34:00) ERROR: /home/blaise/git/bzd/nodejs/core/tests/BUILD:7:17: overwrote runfile nodejs/core/tests/cache_test.mjs, was symlink to bazel-out/k8-dbg-clang-tidy/bin/nodejs/core/tests/cache.install/nodejs/core/tests/cache_test.mjs, now symlink to nodejs/core/tests/cache_test.mjs
- Remove the warnings with vite

## Absolute paths

All assets and sources files can be accessed by their absolute path using the alias `#bzd`.

In some cases, with `nodejs_web`, this is not possible, for example when referencing a path in SCSS with the `url` attribute.
In that case, you must prepend the path with `@`.
