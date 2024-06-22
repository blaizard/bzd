import json as JsonLibrary
import urllib.request
import typing


class Response:

	def __init__(self, response) -> None:
		self.response = response
		self.encoding = "utf8"

	@property
	def status(self):
		return self.response.status

	@property
	def content(self):
		return self.response.read()

	@property
	def text(self):
		return self.content.decode(self.encoding)

	def getHeader(self, name):
		return self.response.getheader(name, None)


class HttpClient:

	@staticmethod
	def get(*args, **kwargs) -> Response:
		return HttpClient._any("get", *args, **kwargs)

	@staticmethod
	def post(*args, **kwargs) -> Response:
		return HttpClient._any("post", *args, **kwargs)

	@staticmethod
	def _any(method: str, url: str, json: typing.Optional[typing.Dict[str, typing.Any]] = None, timeoutS: int = 60) -> Response:

		body = None
		headers = {}

		if json is not None:
			body = JsonLibrary.dumps(json).encode("utf8")
			headers["content-type"] = "application/json; charset=utf-8"
			headers["content-length"] = len(body)

		request = urllib.request.Request(url, data=body, headers=headers)
		response = urllib.request.urlopen(request, timeout=timeoutS)

		return Response(response)
