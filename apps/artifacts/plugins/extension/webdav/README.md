# Webdav

## Test locally

It implements a simple webdav client to interact with the server.

```bash
bazel run apps/artifacts/plugins/extension/webdav:dav -- --endpoint-url "http://localhost:8081/x/memory/webdav" -v cat dav://a.txt
bazel run apps/artifacts/plugins/extension/webdav:dav -- --endpoint-url "http://localhost:8081/x/memory/webdav" -v ls dav://nested
```
