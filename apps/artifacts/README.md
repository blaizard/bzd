# Artifacts

## Run

```bash
bazel run //apps/artifacts/backend --config=dev -- --port 8081

# Test the nodes plugin
bazel run //apps/node_manager:with_bootloader -- --bootloader-uid hello

# Test the docker manager
bazel run //apps/docker_manager -- --docker-socket /run/user/1000/docker.sock --measure-time 1 docker
```

## Run Tests

```bash
bazel run //apps/artifacts/backend --config=dev -- --test
```
