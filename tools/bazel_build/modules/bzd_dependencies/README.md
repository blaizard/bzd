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
