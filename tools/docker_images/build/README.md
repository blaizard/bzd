Docker image used to build tools.
It is necessary to use an old distro, in order to rely on an old glibc. This increases compatibility of the resulting binary with host machines.

# Build

```bash
docker build tools/docker_images/build -t build
```

# Use it

```bash
# Start the docker instance and run a shell with /bash/bin
docker run -it --rm --name my_build -v "$(pwd)"/sandbox:/sandbox build
```

# Stop it

```bash
exit
```
