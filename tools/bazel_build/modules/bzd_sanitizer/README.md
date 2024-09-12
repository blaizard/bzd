# Sanitizer

## Aspects

Some sanitizers, especially static analyzer are more effective as an aspect, as we want to run them as they are invoked in the build.
To use any of the aspects, run bazel with the following (assuming clang_tidy aspects):

```bash
bazel test --aspects="@bzd_sanitizer//aspects/clang_tidy:defs.bzl%clang_tidy_aspect" --output_groups=+report //...
```
