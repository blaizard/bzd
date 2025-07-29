# Ebook

This utility converts an ebook into a PDF.

## Convert an ebook into pdf

Converting an epub with a DRM and keeping the sandbox:

```bash
bazel run apps/ebook -- --sandbox $(pwd)/temp/sandbox --clean --key $(pwd)/temp/Adobe_PrivateLicenseKey--anonymous.der $(pwd)/temp/temp.epub
# or
bazel run apps/ebook -- --sandbox $(pwd)/temp/sandbox --clean $(pwd)/temp/temp.cbr
```

As a prerequisite, it might be necessary to install some dependencies:

```bash
nix-shell -p calibre
```

## Run using the docker image

```bash
bazel run apps/ebook:image.load

# Typical workflow for fnac.com
docker run -it --rm -v .:/sandbox local/image:latest --sandbox /sandbox/temp/sandbox --clean --key /sandbox/temp/Adobe_PrivateLicenseKey--anonymous.der --max-dpi 300 /sandbox/temp/temp.epub
```

Using `--max-dpi 300` gives very good quality pdfs. Perfect for 60ish pages comics, they will be around 60-70MB.
Also anything between 200 and 300 dpi is good.

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
