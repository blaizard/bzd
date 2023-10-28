# Artifacts

## Test Locally

```bash
bazel run apps/artifacts/backend -- --test
```

## Push a new version to docker hub

```bash
bazel run apps/artifacts:push
docker image tag bazel/apps/artifacts:artifacts blaizard/artifacts:latest
docker push blaizard/artifacts:latest
```
