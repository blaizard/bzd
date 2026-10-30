# Rust dependencies

All third-party Rust crates are declared here as `Cargo.toml` manifests, with
their resolved versions in `Cargo.lock`. They are consumed through
`crate.from_cargo`; no Bazel-specific lockfile is needed.

| Bazel repo        | Manifest             | Defined in                     |
| ----------------- | -------------------- | ------------------------------ |
| `@crates`         | `shared/Cargo.toml`  | `shared/shared.MODULE.bazel`   |
| `@crates_std`     | `std/Cargo.toml`     | `std/std.MODULE.bazel`         |
| `@crates_esp32`   | `esp32/Cargo.toml`   | `esp32/esp32.MODULE.bazel`     |
| `@crates_esp32s3` | `esp32s3/Cargo.toml` | `esp32s3/esp32s3.MODULE.bazel` |

`@crates` contains shared `no_std` dependencies usable by every platform.
`@crates_std` contains std-only dependencies such as the std Embassy runtime
and proc-macro crates.

The fragments are included by the root `MODULE.bazel`.

After editing a manifest, regenerate its `Cargo.lock`:

```sh
./tools/bazel query 'set(@crates//:all @crates_std//:all @crates_esp32//:all @crates_esp32s3//:all)'
```

Commit the manifest and the updated `Cargo.lock`.
