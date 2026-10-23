# Dependencies

Dependency updater running dependencies against the workspace.

## Usage

```bash
# Display all discovered dependencies.
bazel run @bzd_dependencies//:bzd_dependencies

# Display only the bazel_dep dependencies.
bazel run @bzd_dependencies//:bzd_dependencies -- --use bazel_dep

# Upgrade all dependencies.
bazel run @bzd_dependencies//:bzd_dependencies -- --upgrade
```

Each dependency is instantiated with the workspace path and implements the `get` / `upgrade` protocol.

## bazel_dep

`bazel_dep` entries are discovered in every `MODULE.bazel` file of the workspace. Upgrading queries the
[Bazel Central Registry](https://registry.bazel.build) and patches each entry to the latest stable version published
there. Modules not published in the registry, such as the local `bzd_*` overrides, are left untouched.

The upgrade requires network access to `https://bcr.bazel.build`. It does not regenerate `MODULE.bazel.lock`, which
Bazel updates on its next build.
