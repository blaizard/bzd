# Webdav

## Test locally

It implements a simple webdav client to interact with the server.

```bash
bazel run apps/artifacts/plugins/extension/webdav:dav -- --endpoint-url "http://localhost:8081/x/memory/webdav" -v cat dav://a.txt
bazel run apps/artifacts/plugins/extension/webdav:dav -- --endpoint-url "http://localhost:8081/x/memory/webdav" -v ls dav://nested
bazel run apps/artifacts/plugins/extension/webdav:dav -- --endpoint-url "http://localhost:8081/x/memory/webdav" -v ls dav://
bazel run apps/artifacts/plugins/extension/webdav:dav -- --endpoint-url "http://localhost:8081/x/memory/webdav" -v du dav://
bazel run apps/artifacts/plugins/extension/webdav:dav -- --endpoint-url "http://localhost:8081/x/memory/webdav" -v rm dav://a.txt

# Accessing protected resources.
bazel run apps/artifacts/plugins/extension/webdav:dav -- --endpoint-url "http://localhost:8081/x/memory_private/webdav" -u "hello" -p "abc123" -v cat dav://a.tx
bazel run apps/artifacts/plugins/extension/webdav:dav -- --endpoint-url "http://localhost:8081/x/memory_private/webdav" -u "hello" -p "abc123" -v ls dav://
```
