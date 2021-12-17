# Vizualize metadata information

Show metadata information of a bazel target.

## Example of usage

```bash
# Build the metadata
bazel build example/bzd/... --output_groups=+metadata
# Visualize
bazel run apps/metadata -- $(pwd)
```
