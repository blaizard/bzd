# Artifacts

## Test Locally

```bash
# Generate the frontend
bazel run apps/artifacts/frontend:local
# Run the backend
bazel run apps/artifacts/backend -- --test
```

## Push a new version to docker hub

```bash
bazel run apps/artifacts
docker image tag bazel/apps/artifacts:artifacts blaizard/artifacts:latest
docker push blaizard/artifacts:latest
```
