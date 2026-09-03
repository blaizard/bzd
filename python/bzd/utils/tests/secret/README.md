To update the secret, run the following command:

```bash
bazel run @bzd_lib//private/secret -- --recipient "age1ywkt9law36r2a854qzme24du9vjsyzz73l4skjc647ffkp4phsuse9afq6" --file $(pwd)/python/bzd/utils/tests/secret/test_secret.txt encrypt
```

To create the key, run the following command:

```bash
bazel run @age//:age-keygen -- -o $(pwd)/python/bzd/utils/tests/secret/test_key.txt
```
