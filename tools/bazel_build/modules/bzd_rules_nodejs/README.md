# NodeJs / NodeJsWeb

## TODO

- Remove the warnings with vite

## Absolute paths

All assets and sources files can be accessed by their absolute path using the alias `#bzd`.

In some cases, with `nodejs_web`, this is not possible, for example when referencing a path in SCSS with the `url` attribute.
In that case, you must prepend the path with `@`.
