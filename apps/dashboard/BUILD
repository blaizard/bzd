load("@io_bazel_rules_docker//container:container.bzl", "container_image")

container_image(
    name = "dashboard",
    base = "@apline_node//image",
    directory = "/data",
    #files = [
    #    "//apps/dashboard/backend:backend.package"
    #],
    tars = [
        "//apps/dashboard/frontend:frontend.package",
        #"//apps/dashboard/backend:backend.package"
        "//example/nodejs:nodejs.package"
    ],
    cmd = ["echo 'hello world'"]
)
