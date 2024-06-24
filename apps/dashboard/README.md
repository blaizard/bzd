# Dashboard

## Test Locally

```bash
bazel run apps/dashboard/backend --config=dev --//apps/dashboard/backend:config.file=//deployment/apps/dashboard_blaizard_com:config.test.json
```

Restart either frontend/backend to rebuild and inject any update.

## Push the docker image

```bash
bazel run apps/dashboard:push --config=prod
```
