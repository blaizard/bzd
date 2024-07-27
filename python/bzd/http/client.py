import json as JsonLibrary
import urllib.request
import typing
import pathlib
import mimetypes
import certifi
import ssl


class Response:

	def __init__(self, response) -> None:
		self.response = response
		self.encoding = "utf8"

	@property
	def status(self) -> int:
		return self.response.status

	@property
	def content(self):
		return self.response.read()

	@property
	def text(self) -> str:
		return self.content.decode(self.encoding)

	def getHeader(self, name: str) -> typing.Optional[str]:
		return self.response.getheader(name, None)


class HttpClient:

	@staticmethod
	def get(*args, **kwargs) -> Response:
		return HttpClient._any("GET", *args, **kwargs)

	@staticmethod
	def post(*args, **kwargs) -> Response:
		return HttpClient._any("POST", *args, **kwargs)

	@staticmethod
	def put(*args, **kwargs) -> Response:
		return HttpClient._any("PUT", *args, **kwargs)

	@staticmethod
	def _any(method: str,
	         url: str,
	         query: typing.Optional[typing.Dict[str, str]] = None,
	         json: typing.Optional[typing.Dict[str, typing.Any]] = None,
	         file: typing.Optional[pathlib.Path] = None,
	         mimetype: typing.Optional[str] = None,
	         timeoutS: int = 60,
	         headers: typing.Optional[typing.Dict[str, str]] = None) -> Response:

		body = None
		headers = headers or {}

		if json is not None:
			body = JsonLibrary.dumps(json).encode("utf8")
			headers["content-type"] = "application/json; charset=utf-8"
			headers["content-length"] = len(body)

		if file is not None:
			body = file.read_bytes()
			headers["content-type"] = mimetype or mimetypes.guess_type(file.name)[0] or "application/octet-stream"
			headers["content-length"] = len(body)

		if query:
			queries = []
			for key, value in query.items():
				if value is not None:
					key = urllib.parse.quote(key)
					value = urllib.parse.quote(value)
					queries.append(f"{key}={value}")
			url += "?" + "&".join(queries)

		context = ssl.create_default_context(cafile=certifi.where())
		request = urllib.request.Request(url, data=body, headers=headers, method=method)
		response = urllib.request.urlopen(request, timeout=timeoutS, context=context)

		return Response(response)
