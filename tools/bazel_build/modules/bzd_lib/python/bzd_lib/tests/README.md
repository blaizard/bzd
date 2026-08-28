To update the secret, run the following command:

```bash
bazel run @bzd_lib//private/secret -- --recipient "age1ywkt9law36r2a854qzme24du9vjsyzz73l4skjc647ffkp4phsuse9afq6" --file $(pwd)/tools/bazel_build/modules/bzd_lib/python/bzd_lib/tests/test_secret.txt encrypt
```

To create the key, run the following command:

```bash
bazel run @age//:age-keygen -- -o $(pwd)/tools/bazel_build/modules/bzd_lib/python/bzd_lib/tests/test_key.txt
```
