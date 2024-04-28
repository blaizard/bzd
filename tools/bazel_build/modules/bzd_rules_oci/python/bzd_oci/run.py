import pathlib
import json
import typing

from bzd_oci.bin import crane
from bzd.utils.run import ExecuteResult, localCommand


def ociPush(path: pathlib.Path, repository: str, **kwargs: typing.Any) -> ExecuteResult:
	# Read the digest
	digest = 1
	return localCommand([crane, "push", str(path), f"{repository}@{digest}"], **kwargs)


#"${REPOSITORY}@${DIGEST}" "${ARGS[@]+"${ARGS[@]}"}" --image-refs "${REFS}"
# DIGEST=$("${YQ}" eval '.manifests[0].digest' "${IMAGE_DIR}/index.json")
