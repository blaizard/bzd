# Dashboard

## Test Locally

```bash
bazel run apps/dashboard/backend --config=dev
```

or

```bash
bazel run deployment/apps/dashboard_blaizard_com:local --config=dev -- --static apps/dashboard/frontend/frontend.bundle
```

Restart either frontend/backend to rebuild and inject any update.

## Push the docker image

```bash
bazel run apps/dashboard:push --config=prod
```
