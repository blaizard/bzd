# Dashboard

## Test Locally

```bash
bazel run apps/dashboard/frontend:dev # Generate the frontend
bazel run apps/dashboard/backend # Run the backend
```

Restart either frontend/backend to rebuild and inject any update.

## Build the docker image

```bash
bazel run apps/dashboard
```
