"""Override the configuration of the job executor application."""

load("@bzd_lib//config:defs.bzl", "make_bzd_config_apply")

bzd_job_executor_oci, _ = make_bzd_config_apply(
    target = Label("//apps/job_executor"),
    configs = [
        Label("//nodejs/vue/apps:config"),
        Label("//apps/job_executor/backend:config"),
        Label("//apps/artifacts/api:config"),
    ],
)

bzd_job_executor_binary, _binary = make_bzd_config_apply(
    target = Label("//apps/job_executor/backend"),
    configs = [
        Label("//nodejs/vue/apps:config"),
        Label("//apps/job_executor/backend:config"),
        Label("//apps/artifacts/api:config"),
    ],
    executable = True,
)
