# Dependencies

Dependency updater running actions against the workspace.

## Usage

```bash
bazel run //:dependencies
```

Each action receives the workspace path as its only argument and is responsible for discovering the files it needs.
