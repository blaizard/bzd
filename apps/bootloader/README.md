# Bootloader

The bootloader covers the following functionalities:

- Self update
- Application launcher
- Application update
- Application rolling back
- Application monitoring

## Starting

```bash
bazel run //apps/bootloader --//apps/artifacts/plugins/fs/release:config.set=urls=hello -- instance0 apps/test
```
