# Python

## Configure

To set the path of some of the binary used by some of the python libraries, you can overwrite the value in `.bazelrc.local` as follow:

```bash
build --@bzd_python//bzd:bin.set=ssh=/run/current-system/sw/bin/ssh
```
