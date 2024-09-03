import typing

from apps.artifacts.api.python.release.release import Update

BinaryData = typing.Dict[str, typing.Any]
TestData = typing.Dict[str, typing.List[BinaryData]]


class Response:

	def __init__(self, data: BinaryData) -> None:
		self.data = data

	@property
	def content(self) -> bytes:
		return self.data["binary"]

	def getHeader(self, key: str) -> typing.Optional[str]:
		headers = {
		    "content-disposition": f"attachment; filename=\"{self.data['name']}\"",
		    "last-modified": "Sat, 28 Mar 2015 08:05:42 GMT"
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

	def fetch(self, path: str, uid: str, ignore: typing.Optional[str] = None) -> typing.Optional[Update]:
		"""Check if there is an update available.

		Args:
			path: The path to be fetched.
			uid: The unique identifier of the caller.
            ignore: The update must ignore a file that contains the provided string.
		"""

		# No data for this path.
		if (path not in self.data) or (len(self.data[path]) == 0):
			return None

		pathData = self.data[path]

		# No argument provided.
		if ignore is None:
			response = Response(pathData[0])

		# Argument provided.
		else:
			index = next((i for i, d in enumerate(pathData) if ignore in d["name"]), None)
			if (index is None) or (index + 1 >= len(pathData)):
				return None
			response = Response(pathData[index + 1])

		return Update(response)
