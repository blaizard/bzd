import typing

from bzd.http.client import HttpClient, HttpResponse


class HttpResponseMock(HttpResponse):
	def __init__(self, status: int, content: bytes, headers: dict[str, str]) -> None:
		self._status = status
		self._content = content
		self._headers = headers

	@property
	def status(self) -> int:
		return self._status

	@property
	def content(self) -> bytes:
		return self._content

	def getHeader(self, name: str) -> str | None:
		return self._headers.get(name, None)


class HttpClientMock:
	def __init__(self, callback: typing.Callable[..., typing.Any]) -> None:
		self.callback = callback

	def _request(
		self,
		method: str,
		url: str,
		body: bytes | None,
		headers: dict[str, str],
		timeoutS: int,
	) -> HttpResponse:
		response = self.callback(method=method, url=url, body=body, headers=headers, timeoutS=timeoutS)
		return HttpClientMock.makeResponse(response)

	@staticmethod
	def makeResponse(
		response: str | bytes | HttpResponse | None,
	) -> HttpResponseMock:
		status = 200
		content = b""
		headers: dict[str, str] = {}

		if isinstance(response, HttpResponseMock):
			return response
		elif isinstance(response, str):
			content = response.encode()
		elif isinstance(response, bytes):
			content = response

		return HttpResponseMock(status=status, content=content, headers=headers)

	def get(self, *args: typing.Any, **kwargs: typing.Any) -> HttpResponse:
		return HttpClient._any(self._request, "GET", *args, **kwargs)

	def post(self, *args: typing.Any, **kwargs: typing.Any) -> HttpResponse:
		return HttpClient._any(self._request, "POST", *args, **kwargs)

	def put(self, *args: typing.Any, **kwargs: typing.Any) -> HttpResponse:
		return HttpClient._any(self._request, "PUT", *args, **kwargs)

	def head(self, *args: typing.Any, **kwargs: typing.Any) -> HttpResponse:
		return HttpClient._any(self._request, "HEAD", *args, **kwargs)

	def delete(self, *args: typing.Any, **kwargs: typing.Any) -> HttpResponse:
		return HttpClient._any(self._request, "DELETE", *args, **kwargs)
