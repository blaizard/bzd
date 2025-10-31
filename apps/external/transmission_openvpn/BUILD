load("//apps/external:defs.bzl", "bzd_docker_binary")

bzd_docker_binary(
    name = "transmission_openvpn",
    docker_compose = ":docker-compose.yml",
    expose = 9091,
)
