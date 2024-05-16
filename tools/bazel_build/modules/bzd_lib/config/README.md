# Config

Configuration build settings are used as follow.

```py
bzd_config(
    name = "config",
    srcs = ["config.json"],
)
```

or

```py
bzd_config(
    name = "config",
    values = {
      "hello": "world",
      "nested.key": "1"
    },
)
```

To update the configuration from a file, use the following syntax:

```sh
bazel build --//path:config.file=//somepath:update_config.json ...
```

The file must have the following syntax:

```json
{
  "<key>": {
    "config": true,
    "nested": {
      "a": 1,
      "b": 2
    }
  }
}
```

Where `<key>` is the doted path of the configuration item to set.

or

```sh
bazel build --//path:config.set=key1=value1 --//path:config.set=key2=value2 ...
```

Additionally, the configuration can be updated with a single well defined build setting //config:override, this helps with transitions for example.

```
bzd_config_override(
  name = "values",
  configs = {
    "//path:my_config": ":config.json",
    "//other/path:my_config": ":config2.json",
  }
)
```

This ensures that `//path:my_config` and `//other/path:my_config` are existing `bzd_config` rules and using `--//config:override=//:values` will override the values.
