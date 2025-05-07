# Build

```bash
docker build tools/docker_images/ebook -t blaizard/ebook:latest
```

# Use

```bash
docker run --name ebook --rm -it blaizard/ebook:latest
```

# Push

```bash
docker push blaizard/ebook:latest
```
