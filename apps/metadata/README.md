# Visualize metadata information

Show metadata information of a bazel target.

## Example of usage

```bash
# Build the metadata
bazel build example/... --output_groups=+metadata --config=linux_x86_64_clang
# Visualize
bazel run apps/metadata -- $(pwd)
```
