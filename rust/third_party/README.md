# Rust dependencies

All third-party Rust crates are declared here as `Cargo.toml` manifests, with
their resolved versions in `Cargo.lock`. They are consumed through
`crate.from_cargo`; no Bazel-specific lockfile is needed.

| Bazel repo        | Manifest             | Defined in                     |
| ----------------- | -------------------- | ------------------------------ |
| `@crates`         | `host/Cargo.toml`    | `host/host.MODULE.bazel`       |
| `@crates_esp32`   | `esp32/Cargo.toml`   | `esp32/esp32.MODULE.bazel`     |
| `@crates_esp32s3` | `esp32s3/Cargo.toml` | `esp32s3/esp32s3.MODULE.bazel` |

The fragments are included by the root `MODULE.bazel`.

After editing a manifest, regenerate its `Cargo.lock`:

```sh
./tools/bazel query 'set(@crates//:all @crates_esp32//:all @crates_esp32s3//:all)'
```

Commit the manifest and the updated `Cargo.lock`.
