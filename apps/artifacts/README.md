# Artifacts

## Run

```bash
bazel run apps/artifacts/backend --config=prod
```

## Test locally

```bash
bazel run apps/artifacts/backend --config=dev --//apps/artifacts/backend:config.file=//apps/artifacts/backend:config.test.json
```
