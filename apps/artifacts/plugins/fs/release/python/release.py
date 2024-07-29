import typing
import pathlib
import struct
import re
import platform
import sys
from datetime import datetime

from bzd.http.client import HttpClient
from apps.artifacts.plugins.fs.release.config import urls

assert len(urls) > 0, f"'urls' from the config cannot be empty."


class Update:

	def __init__(self, response) -> None:
		self.response = response

	@property
	def name(self) -> typing.Optional[str]:
		regexpr = re.compile(r"^.*filename\s*=[\s\"']*(.+?)[\s\"']*$")
		maybeContent = self.response.getHeader("Content-Disposition").lower()
		if maybeContent:
			m = regexpr.match(maybeContent)
			if m:
				return m.group(1)
		return None

	@property
	def lastModified(self) -> typing.Optional[datetime]:
		maybeContent = self.response.getHeader("Last-Modified")
		if maybeContent:
			try:
				return datetime.strptime(maybeContent, "%a, %d %b %Y %H:%M:%S %Z")
			except ValueError:
				pass
		return None

	@property
	def lastModifiedAsString(self) -> typing.Optional[str]:
		return self.response.getHeader("Last-Modified")

	def toFile(self, path: pathlib.Path) -> None:
		path.parent.mkdir(parents=True, exist_ok=True)
		path.write_bytes(self.response.content)


class Release:

	@staticmethod
	def getAlIsa() -> str:
		"""Get the abstraction layer and the isa."""

		# Identify the bit width.
		bitSize = str(8 * struct.calcsize("P"))

		# Identify the instruction set architecture.
		isa = None
		machineStr = platform.machine().lower()
		if any(x in machineStr for x in ("amd", "x86", "i386", "i486", "i586", "i686")):
			isa = f"x86_{bitSize}"
		elif any(x in machineStr for x in ("aarch", "arm")):
			isa = f"arm_{bitSize}"

		# Identify the abstraction layer.
		al = None
		if sys.platform.startswith("linux"):
			al = "linux"
		elif sys.platform.startswith("win") or sys.platform.startswith("cygwin"):
			al = "windows"
		elif sys.platform.startswith("darwin"):
			al = "darwin"

		return al, isa

	def fetch(self, path: str, uid: str, after: typing.Optional[str] = None) -> typing.Optional[Update]:
		"""Check if there is an update available.

        Args:
			path: The path to fetch.
			uid: The unique identifier of the called.
            after: The update must be after this last update filename.
        """

		# Identify the platform
		al, isa = Release.getAlIsa()

		update = None
		for url in urls:
			fullUrl = f"{url}/x/{path}/"

			try:
				response = HttpClient.get(fullUrl, query={"after": after, "uid": uid, "al": al, "isa": isa})
			except Exception as e:
				print(f"Exception while fetching {fullUrl} with error: {e}")
			else:
				# No update available
				if response.status == 204:
					continue

				# An update was found
				update = Update(response)
				break

		if update is None:
			return None

		assert update.name is not None, f"Every update must have a name."
		assert update.lastModified is not None, f"Every update must have a last modification date."
		return update
