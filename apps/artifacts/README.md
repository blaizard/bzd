# Artifacts

## Run

```bash
bazel run apps/artifacts/backend --config=dev -- --port 8081

# Test the nodes plugin
bazel run apps/node_manager:with_bootloader -- --bootloader-uid hello hello
```
