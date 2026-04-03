# Runners

# Linux

Build and Push the image.

```bash
docker build tools/ci/runner/linux -t docker.blaizard.com/tools/ci/runner/linux:latest
docker push docker.blaizard.com/tools/ci/runner/linux:latest
```

# Opencode

Build and Push the image.

```bash
# Build
docker build tools/ci/runner/opencode -t docker.blaizard.com/tools/ci/runner/opencode:latest
# Run locally
docker run -it --rm docker.blaizard.com/tools/ci/runner/opencode:latest /bin/bash
# Push
docker push docker.blaizard.com/tools/ci/runner/opencode:latest
```
