import typing
import pathlib
import struct
import re
import platform
import sys
from datetime import datetime

from bzd.http.client import HttpClient
from bzd.utils.logging import Logger
from apps.artifacts.api.python.common import ArtifactsBase


class Update:

	def __init__(self, response) -> None:
		self.response = response

	@property
	def name(self) -> typing.Optional[str]:
		regexpr = re.compile(r"^.*filename\s*=[\s\"']*(.+?)[\s\"']*$")
		maybeContent = self.response.getHeader("Content-Disposition")
		if maybeContent:
			m = regexpr.match(maybeContent.lower())
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


class Release(ArtifactsBase):

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

	def fetch(self, path: str, uid: str, ignore: typing.Optional[str] = None) -> typing.Optional[Update]:
		"""Check if there is an update available.

        Args:
			path: The path to fetch.
			uid: The unique identifier of the caller.
            ignore: The update must ignore a file that contains the provided string.
        """

		# Identify the platform
		al, isa = Release.getAlIsa()
		query = {"ignore": ignore, "uid": uid, "al": al, "isa": isa}

		def queryToString():
			return "&".join([f"{k}={v}" for k, v in query.items() if v is not None])

		for remote in self.remotes:
			fullUrl = f"{remote}/x/{path}/"

			try:
				response = HttpClient.get(fullUrl, query=query)
			except Exception as e:
				Logger.error(f"Exception while fetching {fullUrl}?{queryToString()}: {str(e)}")
				continue

			# No update available
			if response.status == 204:
				return None

			# An update was found
			update = Update(response)
			if update.name is None:
				Logger.error(
				    f"Every update must have a name, the one from {fullUrl}?{queryToString()} doesn't, ignoring.")
				continue
			if update.lastModified is None:
				Logger.error(
				    f"Every update must have a last modification date, the one from {fullUrl}?{queryToString()} doesn't, ignoring."
				)
				continue
			return update

		return None
