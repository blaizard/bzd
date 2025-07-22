# Ebook

This utility converts an ebook into a PDF.

## Convert an ebook into pdf

Converting an epub with a DRM and keeping the sandbox:

```bash
bazel run apps/ebook -- --sandbox $(pwd)/temp/sandbox --key $(pwd)/temp/Adobe_PrivateLicenseKey--anonymous.der $(pwd)/temp/temp.epub
```

As a prerequisite, it might be necessary to install some dependencies:

```bash
nix-shell -p calibre
```

## Run using the docker image

```bash
bazel run apps/ebook:image.load
# You need to mount the content that is intended to be accessible from docker.
docker run -it --rm -v .:/sandbox local/image:latest --sandbox /sandbox/temp/sandbox --key /sandbox/temp/Adobe_PrivateLicenseKey--anonymous.der /sandbox/temp/temp.epub
```

## Docker

### Use a new docker image

```bash
docker build apps/ebook -t blaizard/ebook:latest
docker push blaizard/ebook:latest
```

Don't forget to update the `digest` field of the `oci_ebook` image in the `MODULE.bazel`.

## Download ebook

### From .acsm files

```bash
nix-shell -p libgourou
adept_activate -a
acsmdownloader my_file.acsm # Downloads the epub.
acsmdownloader --export-private-key # Download the private key.
```
