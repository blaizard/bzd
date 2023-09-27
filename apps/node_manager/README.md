# Node Manager

Create a REST server to manage a node.

## Wake On Lan

Simple utilities to deal with wake on lan.

## Server

Install the server on the host that needs to be woked-up/shutdown.

Copy the file generated by `bazel build apps/node_manager` to the host.

Make it a service as follow:

```bash
cat <<EOF > /etc/systemd/system/bzd.service
[Unit]
Description=bzd Node Manager
After=network.service

[Service]
ExecStart=$(pwd)/node_manager

[Install]
WantedBy=multi-user.target
EOF
chmod 664 /etc/systemd/system/bzd.service
systemctl daemon-reload
systemctl enable bzd.service
systemctl start bzd.service
systemctl status bzd.service
```