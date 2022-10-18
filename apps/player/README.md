# Player

## Test Locally

```bash
# Generate the frontend
bazel run apps/player/frontend:local
# Run the backend
bazel run apps/player/backend -- $(pwd)/apps/player/scenarios/bazel_getting_started_cpp.json
```
