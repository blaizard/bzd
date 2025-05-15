# Ebook

This utility converts an ebook into a PDF.

## Convert an ebook into pdf

If it has a DRM:

```bash
bazel run apps/ebook -- --key $(pwd)/temp/Adobe_PrivateLicenseKey--anonymous.der $(pwd)/temp.epub
```

Otherwise:

```bash
bazel run apps/ebook -- $(pwd)/temp/temp.cbz
```

## Run using the docker image

```bash
bazel run apps/ebook:image.load
# You need to mount the content that is intended to be accessible from docker.
docker run -it --rm -v ./:/sandbox local/image:latest --key /sandbox/adobe_key.der /sandbox/universal.epub
```

## Docker

### Use a new docker image

```bash
docker build apps/ebook -t blaizard/ebook:latest
docker push blaizard/ebook:latest
```

Don't forget to update the `digest` field of the `oci_ebook` image in the `MODULE.bazel`.
