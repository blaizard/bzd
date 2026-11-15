import pathlib
import typing

from bzd.utils.run import ExecuteResult, localCommand

from bzd_oci.bin import crane


def ociPush(path: pathlib.Path, repository: str, **kwargs: typing.Any) -> ExecuteResult:
	return localCommand([crane, "push", str(path), f"{repository}"], **kwargs)
