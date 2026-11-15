import io
import typing

from bzd.http.server import RESTServerContext, RESTServerContextRequest


def makeRESTServerContext(data: bytes = b"", headers: typing.Mapping[str, str] | None = None) -> RESTServerContext:
	"""Build a custom RESTServerContext for testing."""

	if headers is None:
		headers = {
			"Content-Length": str(len(data)),
		}

	request = RESTServerContextRequest(data=io.BytesIO(data), headers=headers)
	return RESTServerContext(request=request)
