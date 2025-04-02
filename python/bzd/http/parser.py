import sys
import typing
import enum
import re

from bzd.utils.timeout import Timeout


class TransferMode(enum.Enum):
	normal = enum.auto()
	chunked = enum.auto()


class HttpParser:

	def __init__(self, reader: typing.Callable[[], bytes], timeoutS: int = 10) -> None:
		"""Generic HTTP Parser.
		
		Args:
			reader: A callable to red a chunk of data.
			timeoutS: Timeout in seconds of a single read request. The whole reading can take longer as long as every read is satisfied within the
			provided timeout.
		"""

		self.reader = reader
		self.timeoutS = timeoutS
		self.buffer = b""
		self.transferMode: typing.Optional[TransferMode] = None
		self.bodyLengthLeft: typing.Optional[int] = None

	@staticmethod
	def getHeader(data: bytes, name: str, defaultValue: typing.Any = None) -> typing.Any:
		"""Read a specific header from a buffer.
		
		Args:
			data: The buffer to read from.
			name: The name of the header.
			defaultValue: The default value returned if there is no header.

		Returns:
			The value of the header or the default value.
		"""

		headerRegex = re.compile(rb"^" + name.encode() + rb":\s*(.*?)(\r\n|$)", re.IGNORECASE | re.MULTILINE)
		match = headerRegex.search(data)
		if match:
			return match.group(1).strip().decode()
		return defaultValue

	def _read(self) -> bytes:
		with Timeout(self.timeoutS):
			return self.reader()

	def _readHeaders(self) -> None:
		"""Process the data and parse the headers.
		
		Returns:
			The part of the body of the HTTP request that was read but not processed.
		"""

		# Read the header.
		while True:
			self.buffer += self._read()
			if b"\r\n\r\n" in self.buffer:
				break
		header, self.buffer = self.buffer.split(b"\r\n\r\n", 1)

		# Read information of the payload.
		transferEncoding = self.getHeader(header, "transfer-encoding")
		if transferEncoding is None:
			self.transferMode = TransferMode.normal
		elif "chunked" in transferEncoding:
			self.transferMode = TransferMode.chunked
		assert self.transferMode is not None, f"Unsupported transfer mode."

		# Read the payload length.
		if self.transferMode == TransferMode.normal:
			contentLength = self.getHeader(header, "content-length")
			assert contentLength is not None, f"Content-length header is missing."
			self.bodyLengthLeft = int(contentLength)

	def _readChunk(self, size: int) -> typing.Generator[bytes, None, None]:
		"""Read a chunk of data by size."""

		while True:
			if self.buffer:
				sizeToRead = min(size, len(self.buffer))
				yield self.buffer[:size]
				size -= sizeToRead
				self.buffer = self.buffer[sizeToRead:]
			if size == 0:
				break
			self.buffer = self._read()

	def read(self) -> typing.Generator[bytes, None, None]:
		self._readHeaders()

		if self.transferMode == TransferMode.normal:
			assert self.bodyLengthLeft is not None, f"Body length information is missing."
			yield from self._readChunk(self.bodyLengthLeft)

		elif self.transferMode == TransferMode.chunked:

			while True:

				# Read the length of the body.
				while True:
					if b"\r\n" in self.buffer:
						line, self.buffer = self.buffer.split(b"\r\n", 1)
						# It can be that we read a empty line.
						if line:
							self.bodyLengthLeft = int(line, 16)
							break
						continue
					self.buffer += self._read()

				if self.bodyLengthLeft == 0:
					break

				yield from self._readChunk(self.bodyLengthLeft)
		else:

			assert False, f"Transfer mode {self.transferMode} is not supported."

	def readAll(self) -> bytes:
		"""Helper to read the whole content."""
		return b"".join([chunk for chunk in self.read()])
