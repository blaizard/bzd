# Dependencies

Dependency updater running dependencies against the workspace.

## Usage

```bash
# Display all discovered dependencies.
bazel run //:dependencies

# Display only the bazel_dep dependencies.
bazel run //:dependencies -- --use bazel_dep

# Upgrade all dependencies.
bazel run //:dependencies -- --upgrade
```

Each dependency is instantiated with the workspace path and implements the `get` / `upgrade` protocol.
