import json as JsonLibrary
import urllib.request
import urllib.parse
import typing
import pathlib
import mimetypes
import certifi
import ssl
import http.client

# ---- Interfaces ----


class HttpClientRequestProtocol(typing.Protocol):

	def __call__(self, method: str, url: str, body: typing.Optional[bytes], headers: typing.Dict[str, str],
	             timeoutS: int) -> "HttpResponse":
		...


# ---- Generic ----


class HttpClientException(Exception):
	"""A custom exception for HTTP client errors."""

	def __init__(self, message: str, status: int, reason: str, content: bytes) -> None:
		super().__init__(message)
		self.status = status
		self.reason = reason
		self.content = content

	def __str__(self) -> str:
		return f"{super().__str__()} [Status Code: {self.status}, Reason: {self.reason}]"


class HttpResponse(typing.Protocol):

	encoding: str = "utf8"

	@property
	def status(self) -> int:
		...

	@property
	def content(self) -> bytes:
		...

	def getHeader(self, name: str) -> typing.Optional[str]:
		...

	@property
	def text(self) -> str:
		return self.content.decode(self.encoding)

	@property
	def json(self) -> typing.Any:
		return JsonLibrary.loads(self.text)


class HttpClient:

	@staticmethod
	def get(*args: typing.Any, **kwargs: typing.Any) -> HttpResponse:
		return HttpClient._any(_request, "GET", *args, **kwargs)

	@staticmethod
	def post(*args: typing.Any, **kwargs: typing.Any) -> HttpResponse:
		return HttpClient._any(_request, "POST", *args, **kwargs)

	@staticmethod
	def put(*args: typing.Any, **kwargs: typing.Any) -> HttpResponse:
		return HttpClient._any(_request, "PUT", *args, **kwargs)

	@staticmethod
	def head(*args: typing.Any, **kwargs: typing.Any) -> HttpResponse:
		return HttpClient._any(_request, "HEAD", *args, **kwargs)

	@staticmethod
	def delete(*args: typing.Any, **kwargs: typing.Any) -> HttpResponse:
		return HttpClient._any(_request, "DELETE", *args, **kwargs)

	@staticmethod
	def _any(request: HttpClientRequestProtocol,
	         method: str,
	         url: str,
	         query: typing.Optional[typing.Dict[str, str]] = None,
	         json: typing.Optional[typing.Dict[str, typing.Any]] = None,
	         file: typing.Optional[pathlib.Path] = None,
	         mimetype: typing.Optional[str] = None,
	         timeoutS: int = 60,
	         headers: typing.Optional[typing.Dict[str, str]] = None) -> HttpResponse:

		body = None
		headers = headers or {}

		if json is not None:
			body = JsonLibrary.dumps(json).encode("utf8")
			headers["content-type"] = "application/json; charset=utf-8"
			headers["content-length"] = str(len(body))

		if file is not None:
			body = file.read_bytes()
			headers["content-type"] = mimetype or mimetypes.guess_type(file.name)[0] or "application/octet-stream"
			headers["content-length"] = str(len(body))

		if query:
			queries = []
			for key, value in query.items():
				if value is not None:
					queries.append(f"{urllib.parse.quote(key)}={urllib.parse.quote(str(value))}")
			url += "?" + "&".join(queries)

		return request(method=method, url=url, body=body, headers=headers, timeoutS=timeoutS)


# ---- Implementation ----


class _HTTPResponseAdapter(HttpResponse):

	def __init__(self, response: http.client.HTTPResponse) -> None:
		self.response = response

	@property
	def status(self) -> int:
		return self.response.status

	@property
	def content(self) -> bytes:
		return self.response.read()

	def getHeader(self, name: str) -> typing.Optional[str]:
		return self.response.getheader(name, None)


def _request(method: str, url: str, body: typing.Optional[bytes], headers: typing.Dict[str, str],
             timeoutS: int) -> HttpResponse:
	"""Processor of a request using urllib."""

	context = ssl.create_default_context(cafile=certifi.where())
	request = urllib.request.Request(url, data=body, headers=headers, method=method)

	try:
		response = urllib.request.urlopen(request, timeout=timeoutS, context=context)
		return _HTTPResponseAdapter(response)

	except urllib.error.HTTPError as e:
		bodyError = e.read()
		raise HttpClientException(
		    message=f"HTTP Error: {e.reason} ({e.code}) calling {url}: response body: {bodyError.decode('utf-8')}",
		    status=e.code,
		    reason=e.reason,
		    content=bodyError) from e

	except urllib.error.URLError as e:
		raise Exception(f"URL Error: {e.reason} calling {url}")
