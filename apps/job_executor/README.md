# Job Executor

## Test Locally

```bash
bazel run apps/job_executor/backend --config=dev
```

## Push the docker image

```bash
bazel run apps/job_executor:push --config=prod
```
