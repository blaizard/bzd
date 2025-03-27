# Webdav

## Test locally

It implements a simple webdav client to interact with the server.

```bash
bazel run apps/artifacts/plugins/extension/webdav:dav -- --endpoint-url "http://localhost:8081/x/test/webdav" -v cat dav://api.json
```
