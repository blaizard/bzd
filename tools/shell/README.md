# Shell

## Generate locally

To re-generate the shell, use the following. Note this is also done as part of the sanitizer.

```sh
bazel run @bzd_sanitizer//:sanitizer -- --use //tools/shell:generate
```

## Automatic install

This tool provides helpful commands to enhance the current shell, to install it simply run the following:

```sh
bazel run //tools/shell:install
source ~/.bashrc
```

## Manual install

### SH

SH-like environment can be installed manually and hooked to `.bashrc` for a specific user.
This is done with the following command:

```sh
cd ~
mkdir -p .bzd
wget "https://raw.githubusercontent.com/blaizard/cpp-async/master/tools/shell/sh/bashrc.sh" -O .bzd/bashrc.sh
echo "source .bzd/bashrc.sh" > .bashrc
source .bashrc
```
