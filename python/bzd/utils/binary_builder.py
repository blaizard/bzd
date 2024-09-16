import pathlib
import argparse
import typing
import dataclasses


@dataclasses.dataclass
class Chunk:
	"""Class defining a chunk."""

	path: pathlib.Path
	address: int
	maxSize: typing.Optional[int]

	@property
	def isMaxSize(self) -> bool:
		return self.maxSize is not None


@dataclasses.dataclass
class Binary:
	"""Class to define a binary."""

	chunks: typing.List[Chunk]
	size: typing.Optional[int] = None

	@property
	def isSize(self) -> bool:
		return self.size is not None

	def toFile(self, path: pathlib.Path) -> None:
		"""Create a binary and write to a file."""

		with open(path, "wb") as fout:

			# Create the full file.
			if self.isSize:
				assert self.size is not None
				fout.seek(self.size - 1)
				fout.write(b"\0")

			# Compose the binary with content from the chunks.
			for chunk in self.chunks:
				with open(chunk.path, "rb") as fin:
					fout.seek(chunk.address)
					fout.write(fin.read())
					if chunk.isMaxSize:
						assert chunk.maxSize is not None
						assert fin.tell(
						) <= chunk.maxSize, f"The chunk '{chunk}' is too large ({fin.tell()}) to fit into the binary."


def readSize(arg: str) -> int:
	"""Convert a size argument from a string to a number.
	
	Accepted values are for example: 12, 0x12, 12KB, 12MB and 12GB.
	"""

	try:
		return int(arg, 0)
	except ValueError:
		if arg.endswith("B"):
			arg = arg[:-1]
			if arg.endswith("K"):
				return int(arg[:-1]) * 1024
			elif arg.endswith("M"):
				return int(arg[:-1]) * 1024 * 1024
			elif arg.endswith("G"):
				return int(arg[:-1]) * 1024 * 1024 * 1024
		raise


def makeBinary(chunks: typing.Iterable[str], size: typing.Optional[str] = None) -> Binary:
	"""Convert a string representation of a binary into a binary."""

	binary = Binary(chunks=[], size=None if size is None else readSize(size))

	for chunk in chunks:
		parts = chunk.split(",")
		assert len(
		    parts
		) >= 2, f"--chunk needs at least the path and the address in the following format: 'path,address', not '{chunk}'."
		assert len(parts) <= 3, f"--chunk cannot have more than 3 entities: 'path,address,max-size', not '{chunk}'."

		binary.chunks.append(
		    Chunk(pathlib.Path(parts[0]), int(parts[1], 0),
		          int(parts[2], 0) if len(parts) == 3 else None))

	# Sort the chunks by their addresses.
	binary.chunks.sort(key=lambda x: x.address)

	# Update the max size.
	nextChunkIndex = 0
	for nextChunkIndex in range(1, len(binary.chunks)):
		chunkObject = binary.chunks[nextChunkIndex - 1]
		nextChunk = binary.chunks[nextChunkIndex]

		availableSize = nextChunk.address - chunkObject.address
		if chunkObject.isMaxSize:
			assert chunkObject.isMaxSize <= availableSize, f"The maximum size defined for {chunkObject}, is larger the available size ({availableSize}) between chunks (next chunk starts at address {nextChunk.address})."
		else:
			chunkObject.maxSize = availableSize

	# Compare the last chunk size with the maxSize if defined.
	if len(binary.chunks) > 0 and binary.isSize:
		chunkObject = binary.chunks[nextChunkIndex]
		availableSize = binary.size - chunkObject.address
		assert availableSize > 0, f"There is no size available for the last chunk: {availableSize} given the size constraint of {binary.size}."
		if chunkObject.isMaxSize:
			assert chunkObject.isMaxSize <= availableSize, f"The maximum size defined for {chunkObject}, is larger the available size ({availableSize}) for the last chunk given the size constraint of {binary.size}."
		else:
			chunkObject.maxSize = availableSize

	return binary


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Assemble binary parts together.")
	parser.add_argument("--size", default=None, help="The total size of the binary in bytes.")
	parser.add_argument(
	    "--chunk",
	    dest="chunks",
	    action="append",
	    default=[],
	    help="Add chunks at specific addresses using the following format: 'path,address[,max-size]'.",
	)
	parser.add_argument("output", type=pathlib.Path, help="Path of the file to be generated.")

	args = parser.parse_args()

	binary = makeBinary(chunks=args.chunks, size=args.size)
	binary.toFile(args.output)
