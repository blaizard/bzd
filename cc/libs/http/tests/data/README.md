# HTTP Data

The raw data have been capture with the following command:

```bash
curl --include --raw https://www.google.com > file.raw
```

Be carefll with editors like `vscode` that modify the line ending upon save. Better use `vi` for example to edit the file.

Note, that `vi` might insert a new line at the end of the file, this can be removed with:

```bash
truncate -s -1 file.raw
```
