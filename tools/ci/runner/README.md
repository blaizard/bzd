# Runners

# Linux

Build and Push the image.

```bash
# Build
bazel run tools/ci/runner:linux -- --no-cache -t docker.blaizard.com/tools/ci/runner/linux:latest
# Run locally
docker run -it --rm -v .:/workspace --workdir /workspace docker.blaizard.com/tools/ci/runner/linux:latest /bin/bash
# Push
docker push docker.blaizard.com/tools/ci/runner/linux:latest
```

# Opencode

Build and Push the image.

```bash
# Build
bazel run tools/ci/runner:opencode -- -t docker.blaizard.com/tools/ci/runner/opencode:latest
# Run locally
docker run -it --rm -v .:/workspace --workdir /workspace docker.blaizard.com/tools/ci/runner/opencode:latest /bin/bash
# Push
docker push docker.blaizard.com/tools/ci/runner/opencode:latest
```
