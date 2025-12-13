# Ebook

This utility converts an ebook into a PDF.

## Convert an ebook into pdf

Converting an epub with a DRM and keeping the sandbox:

```bash
bazel run apps/ebook -- --sandbox $(pwd)/temp/sandbox --clean --max-dpi 200 --key $(pwd)/temp/Adobe_PrivateLicenseKey--anonymous.der pdf $(pwd)/temp/temp.epub
# or
bazel run apps/ebook -- --sandbox $(pwd)/temp/sandbox --clean --max-dpi 200 pdf $(pwd)/temp/temp.pdf
# or
bazel run apps/ebook -- --max-dpi 300 pdf $(pwd)/temp/
# or
bazel run apps/ebook -- cover $(pwd)/temp/
```

As a prerequisite, it might be necessary to install some dependencies:

```bash
nix-shell -p calibre
```

## Run using the docker image

```bash
bazel run apps/ebook:image.load

# Typical workflow for fnac.com
docker run -it --rm -v .:/sandbox local/image:latest --key /sandbox/temp/Adobe_PrivateLicenseKey--anonymous.der --max-dpi 200 pdf "/sandbox/temp/temp.epub"

# Typical workflow for a .cbz
docker run -it --rm -v .:/sandbox local/image:latest --max-dpi 300 pdf "/sandbox/temp/temp.cbz"

# Typical workflow for a .pdf
docker run -it --rm -v .:/sandbox local/image:latest --max-dpi 300 pdf "/sandbox/temp/temp.pdf"
```

Using `--max-dpi 300` gives very good quality pdfs. Perfect for 60ish pages comics, they will be around 60-70MB.
Also anything between 200 and 300 dpi is good.

## Docker

### Use and push a new base docker image

This is the image that will be used as a base for the application, this is NOT the application.

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
