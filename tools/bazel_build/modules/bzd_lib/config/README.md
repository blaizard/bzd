# Config

Configuration build settings are used as follow.

```py
bzd_config(
    name = "config",
    srcs = ["config.json"],
)
```

To update the configuration, use the following syntax:

```sh
bazel build --//path:config.file=//somepath:update_config.json ...
```

or

```sh
bazel build --//path:config.set=key1=value1 --//path:config.set=key2=value2 ...
```
