# Gitea

## Runners

### Build and Publish

To build and push the runner:

```bash
bazel run //deployment/ci/gitea:node_manager_client_runner_push --config=prod
```
