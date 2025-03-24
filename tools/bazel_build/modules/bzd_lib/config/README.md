# Config

Configuration build settings are used as follow.

```py
bzd_config_default(
    name = "config",
    srcs = ["config.json"],
)
```

or

```py
bzd_config_default(
    name = "config",
    values = {
      "hello": "world",
      "nested.key": "1" # This notation expands the '.' as `{nested: {key: 1}}`
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

Additionally, the configuration can be applied to a rule with `bzd_config_apply` or its variants, as follow for example.

```py
bzd_config_apply(
  name = "my_target_with_config",
  configs = {
    "//path:my_config": ":config.json",
    "//other/path:my_config": ":config2.json",
  },
  target = "//:my_target"
)
```

## Adding data to a configuration

Configuration can be associated with runtime data that will be added to the runfiles.
For example:

```py
bzd_config_default(
    name = "config",
    values = {
      "path": "$(location //my/target)",
    },
    data = [
      "//my/target"
    ]
)

*_binary(
  ...
  srcs = [
    ":config.json" # To access the config value with:`from config import path`.
  ],
  data = [
    ":config" # To access the files associated with the target at runtime.
  ]
)
```

## Design Decisions

The overwrite mechanism directly update the specific lable_flag that is registered through the extensions.

This design is different from the original design, which was creating a directionary with keys equal to config default targets and values equals to config values.
This dictionary was then passed to a global label flag, which all config defaults were depending on. This had the advantage to not needing the extensions, which is somewhat redundant. The big drawback is that every default config would depend on every config overwrite, which may lead to dependency loops in some cases.
