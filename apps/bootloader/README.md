# Bootloader

The bootloader covers the following functionalities:

- Self update
- Application launcher
- Application update
- Application rolling back
- Application monitoring

## Starting

```bash
bazel run //apps/bootloader --//apps.artifacts.plugins.extension.release:config.set=urls=hello -- instance0 apps/test
```
