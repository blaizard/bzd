# Dashboard

## Test Locally

```bash
bazel run apps/dashboard/backend --config=dev
```

Restart either frontend/backend to rebuild and inject any update.

## Push the docker image

```bash
bazel run apps/dashboard:push
```
