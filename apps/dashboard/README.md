# Dashboard

## Test Locally

```bash
# Generate the frontend
bazel run apps/dashboard/frontend:local
# Run the backend
bazel run apps/dashboard/backend
```

Restart either frontend/backend to rebuild and inject any update.

## Build the docker image

```bash
bazel run apps/dashboard
```
