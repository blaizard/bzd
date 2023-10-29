# Artifacts

## Test Locally

```bash
bazel run apps/artifacts/backend --config=dev -- --test
```

## Push a new version to docker hub

```bash
bazel run apps/artifacts:push
```
