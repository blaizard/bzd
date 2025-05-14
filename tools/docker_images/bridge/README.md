# Bridge

## Build

```bash
docker build apps/bridge -t blaizard/bridge:latest
```

## Push

```bash
docker push blaizard/bridge:latest
```

## Inspect

```bash
docker run --rm -it --entrypoint /bin/bash blaizard/bridge:latest -c "cat /etc/ssh/sshd_config"
```
