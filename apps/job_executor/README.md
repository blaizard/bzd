# Job Executor

## Test Locally

```bash
# Load a test docker image in memory.
bazel run apps/job_executor/tests:image_sleep.load

# Run the application.
bazel run apps/job_executor/backend --config=dev
```

## Push the docker image

```bash
bazel run apps/job_executor:push --config=prod
```
