This is a test

```bash
bazel fetch --experimental_downloader_config=$(pwd)/apps/artifacts/plugins/extension/caching_proxy/downloader.cfg --repository_cache= --http_timeout_scaling=4.0 --target_pattern_file=tools/ci/bazel_target_patterns.txt
```
