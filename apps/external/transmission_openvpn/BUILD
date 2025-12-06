load("//apps/external:defs.bzl", "bzd_docker_binary")

exports_files(["docker-compose.yml"])

bzd_docker_binary(
    name = "transmission_openvpn",
    docker_compose = ":docker-compose.yml",
    expose = 9091,
)
