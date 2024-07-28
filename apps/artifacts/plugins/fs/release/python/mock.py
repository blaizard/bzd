import typing

from apps.artifacts.plugins.fs.release.python.release import Update

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

	def __init__(self, uid: str, data: TestData) -> None:
		"""Args:
			uid: The unique identifier of the caller.
			data: Mocking data with the following format:
			{
				<app>: [
					{name: "first", binary: <binary>}, // will be returned first
					{name: "second", binary: <binary>} // then this one if it's called with the previous one, etc.
				]
			}
		"""
		self.uid = uid
		self.data = data

	def fetch(self, app: str, after: typing.Optional[str] = None) -> typing.Optional[Update]:
		"""Check if there is an update available.

		Args:
			app: The application to be fetched.
			after: The update must be after this last update filename.
		"""

		# No data for this app.
		if (app not in self.data) or (len(self.data[app]) == 0):
			return None

		appData = self.data[app]

		# No argument provided.
		if after is None:
			response = Response(appData[0])

		# Argument provided.
		else:
			index = next((i for i, d in enumerate(appData) if d["name"] == after), None)
			if (index is None) or (index + 1 >= len(appData)):
				return None
			response = Response(appData[index + 1])

		return Update(response)
