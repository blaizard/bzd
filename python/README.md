# Python

## Configure

To set the path of some binaries used by the Python libraries, you can override the value in `.bazelrc.local` as follows:

```bash
build --@bzd_python//bzd:bin.set=ssh=/run/current-system/sw/bin/ssh
```
