# Ebook

This utility converts an ebook into a PDF.

## Convert an ebook into pdf

If it has a DRM:

```bash
bazel run tools/docker_images/ebook -- --key $(pwd)/temp/Adobe_PrivateLicenseKey--anonymous.der $(pwd)/temp.epub
```

Otherwise:

```bash
bazel run tools/docker_images/ebook -- --sandbox $(pwd)/sandbox $(pwd)/temp/temp.cbz
```

## Docker

### Build

```bash
docker build tools/docker_images/ebook -t blaizard/ebook:latest
```

### Use

```bash
docker run --name ebook --rm -it blaizard/ebook:latest
```

### Push

```bash
docker push blaizard/ebook:latest
```
