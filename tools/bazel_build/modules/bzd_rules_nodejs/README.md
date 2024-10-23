# NodeJs / NodeJsWeb

## TODO

- Fix pnpm store-dir to use a specific or configurable path.
- Remove the warnings with vite

## Absolute paths

All assets and sources files can be accessed by their absolute path using the alias `#bzd`.

In some cases, with `nodejs_web`, this is not possible, for example when referencing a path in SCSS with the `url` attribute.
In that case, you must prepend the path with `@`.

## Design

Adopt a design similar to rules_python to handle dependencies.

Describe all the dependencies repo-wise in a single file:

```txt
mocha=10.0.0
```

Use a tool (can be wrapped into a rule), to convert this into a .lock files that includes the deps and hashes.

```py
deps = use_extension("@bzd_rules_nodejs//:deps.bzl", "deps")
deps.parse(
    name = "nodejs_deps",
    requirements_lock = "//tools/nodejs:requirements.lock",
)
use_repo(deps, "nodejs_deps")
```

When we want to depend on such dependency, we do:

```py
deps = [
	"@nodejs_deps//:mocha"
]
```

`@nodejs_deps//:mocha` is an alias to a sub-repository of `@nodejs_deps//` that, when accessed, will fetch the dependencies
and their dependencies into the pnpm store:

```bash
pnpm store add mocha@10.0.0 ...
```

(where ... is all the deps).

When building a target that depends on these packages, it aggregates them all, create a package.json file, and use

```bash
pnpm add --offline mocha@10.0.0...
```

to generate the node_modules directory.
