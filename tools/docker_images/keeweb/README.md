# Build

```bash
docker build apps/keeweb -t blaizard/keeweb:latest
```

# Use

```bash
docker run --name keeweb --rm -p 8080:80 blaizard/keeweb:latest
```

# Push

```bash
docker push blaizard/keeweb:latest
```
