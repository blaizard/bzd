import pathlib
import typing

from bzd_oci.bin import crane
from bzd.utils.run import ExecuteResult, localCommand


def ociPush(path: pathlib.Path, repository: str, **kwargs: typing.Any) -> ExecuteResult:
	return localCommand([crane, "push", str(path), f"{repository}"], **kwargs)
