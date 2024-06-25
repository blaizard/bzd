# Artifacts

## Run

```bash
bazel run apps/artifacts/backend --config=prod
```

## Test locally

```bash
bazel run apps/artifacts/backend --config=dev --//apps/artifacts/backend:config.file=//deployment/apps/data_blaizard_com:config.test.backend.json
```
