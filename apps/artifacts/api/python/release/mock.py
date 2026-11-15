import typing

from apps.artifacts.api.python.release.release import Update
from bzd.http.utils import encodeURIComponent

BinaryData = dict[str, typing.Any]
TestData = dict[str, list[BinaryData]]


class Response:
	def __init__(self, data: BinaryData) -> None:
		self.data = data

	@property
	def content(self) -> bytes:
		return typing.cast(bytes, self.data["binary"])

	def getHeader(self, key: str) -> str | None:
		headers = {
			"content-disposition": f'attachment; filename="{encodeURIComponent(self.data["name"])}"',
			"last-modified": "Sat, 28 Mar 2015 08:05:42 GMT",
		}
		return headers.get(key.lower(), None)


class ReleaseMock:
	def __init__(self, data: TestData) -> None:
		"""Args:
		data: Mocking data with the following format:
		{
		        <path>: [
		                {name: "first", binary: <binary>}, // will be returned first
		                {name: "second", binary: <binary>} // then this one if it's called with the previous one, etc.
		        ]
		}
		"""
		self.data = data

	def fetch(
		self,
		path: str,
		uid: str | None = None,
		ignore: str | None = None,
	) -> Update | None:
		"""Check if there is an update available.

		    Args:
		            path: The path to be fetched.
		            uid: The unique identifier of the caller.
		ignore: The update must ignore a file that contains the provided string.
		"""

		# No data for this path.
		if len(self.data.get(path, [])) == 0:
			return None

		pathData = self.data[path]

		# No argument provided.
		if ignore is None:
			response = Response(pathData[0])

		# Argument provided.
		else:
			maybeData = next((data for data in pathData if ignore not in data["name"]), None)
			if maybeData is None:
				return None
			response = Response(maybeData)

		return Update(response)
