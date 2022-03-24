# Bridge

First of all to install all tools, do the following:

```sh
export DEBIAN_FRONTEND=noninteractive
apt install -y docker.io docker-compose wget
wget "https://raw.githubusercontent.com/blaizard/cpp-async/master/tools/docker_images/bridge/docker-compose.yml" -O "docker-compose.yml"
```

Then register the container as a service and run it.

```sh
cat <<EOF > /etc/systemd/system/bridge.service
[Unit]
Description=bzd Bridge
After=network.service

[Service]
ExecStart=docker-compose -f $(pwd)/docker-compose.yml up

[Install]
WantedBy=multi-user.target
EOF
chmod 664 /etc/systemd/system/bridge.service
systemctl daemon-reload
systemctl enable bridge.service
systemctl start bridge.service
systemctl status bridge.service
```
