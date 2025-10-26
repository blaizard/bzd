load("//:defs.bzl", "bzd_diff_test")
load("//config:defs.bzl", "bzd_config", "bzd_config_default")
load("//config/tests:defs.bzl", "bzd_config_apply_test")

# ---- Simple ----

bzd_config_default(
    name = "config_merge",
    srcs = [
        "a.yaml",
        "b.json",
    ],
    data = [
        "data.txt",
    ],
    srcs_at = {
        "b.json": "c.nested",
        "c.json": "c.nested,c.nested2",
    },
    values = {
        "d.value": "hello",
        "data": "$(location data.txt)",
    },
)

bzd_diff_test(
    name = "config_merge_json_test",
    file1 = ":config_merge.json",
    file2 = "expected_config_merge.json",
    format = "json",
)

bzd_diff_test(
    name = "config_merge_yaml_test",
    file1 = ":config_merge.yaml",
    file2 = "expected_config_merge.yaml",
    format = "text",
)

bzd_diff_test(
    name = "config_merge_py_test",
    file1 = ":config_merge.py",
    file2 = "expected_config_merge.py",
    format = "text",
)

# ---- Override some values ----

bzd_config(
    name = "config_override",
    srcs_at = {
        "b.json": "a",
    },
    values = {
        "c.nested.c": "wrong",
        "d.value": "world",
    },
)

bzd_diff_test(
    name = "_config_merge_override_test",
    file1 = ":config_merge.json",
    file2 = "expected_config_merge_override.json",
    format = "json",
    tags = ["manual"],
)

bzd_config_apply_test(
    name = "config_merge_override_test",
    configs = {
        ":config_merge": ":config_override",
    },
    target = ":_config_merge_override_test",
)
