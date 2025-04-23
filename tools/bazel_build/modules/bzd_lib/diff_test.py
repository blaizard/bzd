import argparse
import pathlib
import typing
import sys
import enum
import dataclasses

# Take a value of 0 if there is no diff, or a running number that corresponds to the
# match in both contents.
IsDiff = int
DiffContent = typing.List[typing.Tuple[int, IsDiff]]


@dataclasses.dataclass
class Delta:
	"""Container to hold a diff delta."""

	# Offset in the first content.
	offset1: int = -1
	# Offset in the second content.
	offset2: int = -1
	# Diff of the first content.
	diff1: DiffContent = dataclasses.field(default_factory=lambda: [])
	# Diff of the second content.
	diff2: DiffContent = dataclasses.field(default_factory=lambda: [])

	@property
	def isEmpty(self) -> bool:
		return self.offset1 == -1 and self.offset2 == -1

	def addDiff1(self, content: bytes, isMatch: IsDiff) -> None:
		self.diff1 += [(
		    c,
		    isMatch,
		) for c in content]

	def addDiff2(self, content: bytes, isMatch: IsDiff) -> None:
		self.diff2 += [(
		    c,
		    isMatch,
		) for c in content]


class VisitorPrint:

	def __init__(self, file1: str = "file1", file2: str = "file2", color: bool = True, width: int = 80) -> None:
		"""Create a visitor to go through diff deltas.
		
		Args:
			file1: File name of the first file to compare.
			file2: File name of the second file to compare.
			color: If output should be colored.
			width: The width of the output.
		"""
		self.file1 = file1
		self.file2 = file2
		self.color = color
		self.width = width

	@staticmethod
	def getColorAsString(isDiff: IsDiff) -> str:
		colors = ["\033[0;31m", "\033[0;33m", "\033[0;34m", "\033[0;35m"]
		if isDiff > 0:
			return colors[(isDiff - 1) % len(colors)]
		return ""

	def toHexAsString(self, diff: DiffContent, padding: typing.Optional[int] = None) -> str:
		"""Convert a diff into its hexadecimal representation.
		
		Args:
			diff: The diff to convert.
			padding: The padding in term of characters.

		Returns:
			The string representing the diff.
		"""

		if self.color:
			content = ["{}{:02x}\033[0m".format(VisitorPrint.getColorAsString(isDiff), byte) for byte, isDiff in diff]
		else:
			content = ["{:02x}".format(byte) for byte, _ in diff]
		if padding is not None:
			content += ["  "] * max(0, padding - len(diff))
		return " ".join(content)

	def toTextAsString(self, diffContent: DiffContent, padding: typing.Optional[int] = None) -> str:
		"""Convert a diff into its text representation.
		
		Args:
			diff: The diff to convert.
			padding: The padding in term of characters.

		Returns:
			The string representing the diff.
		"""

		if self.color:
			content = [
			    "{}{}\033[0m".format(VisitorPrint.getColorAsString(isDiff),
			                         chr(byte) if byte in range(32, 127) else ".") for byte, isDiff in diffContent
			]
		else:
			content = ["{}".format(chr(byte) if byte in range(32, 127) else ".") for byte, _ in diffContent]
		if padding is not None:
			content += [" "] * max(0, padding - len(diffContent))
		return "".join(content)

	@staticmethod
	def chunkedIterator(diffContent1: DiffContent, diffContent2: DiffContent,
	                    chunkSize: int) -> typing.Generator[typing.Tuple[DiffContent, DiffContent], None, None]:
		"""Iterates through two diff content buffers in chunks of a specified size.

		Args:
			diffContent1: The first diff content buffer.
			diffContent2: The second diff content buffer.
			chunkSize: The desired size of each chunk.

		Yields:
			A tuple containing chunks of size 'chunkSize' from diffContent1 and diffContent2.
			The last chunk might be shorter if the buffer length is not a multiple of chunkSize.
		"""
		len1 = len(diffContent1)
		len2 = len(diffContent2)
		for i in range(0, max(len1, len2), chunkSize):
			chunk1 = diffContent1[i:min(i + chunkSize, len1)]
			chunk2 = diffContent2[i:min(i + chunkSize, len2)]
			yield chunk1, chunk2

	def visitBinary(self, delta: Delta) -> None:
		"""Print the delta as for a binary.
		
		Args:
			delta: The delta to be printed.
		"""

		formatLine = "{}|{}"
		# Number of bytes to print per line.
		chunkSize = int((self.width - len(formatLine.format("", "")) - 1) / 4)

		left: typing.List[str] = [f"== {self.file1}@{delta.offset1}"]
		right: typing.List[str] = [f"== {self.file2}@{delta.offset2}"]

		for chunk1, chunk2 in VisitorPrint.chunkedIterator(delta.diff1, delta.diff2, chunkSize):
			left.append(
			    formatLine.format(self.toHexAsString(chunk1, padding=chunkSize),
			                      self.toTextAsString(chunk1, padding=chunkSize)))
			right.append(
			    formatLine.format(self.toHexAsString(chunk2, padding=chunkSize),
			                      self.toTextAsString(chunk2, padding=chunkSize)))

		print("=" * self.width)
		print("\n".join(left))
		print("\n".join(right))

	def visitText(self, delta: Delta) -> None:
		"""Print the delta as for a text.
		
		Args:
			delta: The delta to be printed.
		"""

		left: typing.List[str] = [f"== {self.file1}@{delta.offset1}"]
		right: typing.List[str] = [f"== {self.file2}@{delta.offset2}"]

		for chunk1, chunk2 in VisitorPrint.chunkedIterator(delta.diff1, delta.diff2, self.width):
			left.append(self.toTextAsString(chunk1))
			right.append(self.toTextAsString(chunk2))

		print("=" * self.width)
		print("\n".join(left))
		print("\n".join(right))


class Diff:
	"""Create a diff between 2 buffers."""

	def __init__(self, content1: bytes, content2: bytes) -> None:
		self.content1 = content1
		self.content2 = content2
		self._preprocess()

	@property
	def isMatch(self) -> bool:
		"""If the 2 content are matching."""
		return len(self.diffs) == 0

	def _preprocess(self) -> None:
		"""Run the algorithm to compute the diffs."""

		lcs = self.lcs()
		index1 = 0
		index2 = 0
		self.diffs: typing.List[typing.Tuple[int, int, int, int]] = []

		# Create the list of diffs
		for b in lcs:
			nextIndex1 = content1.find(b, index1)
			nextIndex2 = content2.find(b, index2)

			# If there is a difference.
			if nextIndex1 != index1 or nextIndex2 != index2:
				self.diffs.append((
				    index1,
				    nextIndex1,
				    index2,
				    nextIndex2,
				))

			index1 = nextIndex1 + 1
			index2 = nextIndex2 + 1

		if index1 < len(self.content1) or index2 < len(self.content2):
			self.diffs.append((
			    index1,
			    len(self.content1),
			    index2,
			    len(self.content2),
			))

	def visit(self, visitor: typing.Callable[[Delta], None], context: int = 4, distance: int = 8) -> None:
		"""Visit the diff.
		
		Args:
			visitor: Called each time a new delta needs to be processed.
			context: Number of character to be added as a context before and after the delta.
			distance: Maximum distance in character to consider 2 deltas as one.
		"""

		# Combine and present diffs.
		previousIndex1 = 0
		previousIndex2 = 0
		isDiffCounter = 1
		delta = Delta()
		for index1, nextIndex1, index2, nextIndex2 in self.diffs:

			publishDiff = False

			# First diff.
			if delta.isEmpty:

				# Add the context
				delta.offset1 = index1
				delta.offset2 = index2
				delta.addDiff1(self.content1[index1 - min(index1 - previousIndex1, context):index1], 0)
				delta.addDiff2(self.content2[index2 - min(index2 - previousIndex2, context):index2], 0)

			# Diff exists but close, can concatenate.
			elif index1 - previousIndex1 <= distance:
				delta.addDiff1(self.content1[index1 - min(index1 - previousIndex1, distance):index1], 0)
				delta.addDiff2(self.content2[index2 - min(index2 - previousIndex2, distance):index2], 0)

			# Diff exists.
			else:
				publishDiff = True

			# Add the diff
			delta.addDiff1(self.content1[index1:nextIndex1], isDiffCounter)
			delta.addDiff2(self.content2[index2:nextIndex2], isDiffCounter)
			isDiffCounter += 1

			if publishDiff:
				visitor(delta)
				# Reset the various variables.
				delta = Delta()
				isDiffCounter = 1

			# Update the indexes
			previousIndex1 = nextIndex1
			previousIndex2 = nextIndex2

		if not delta.isEmpty:
			visitor(delta)

	def lcs(self) -> bytes:
		"""Finds a common subsequence of two bytes sequences using a greedy approach.
		Note: This does not guarantee the *longest* common subsequence.

		Returns:
			The guessed longest common subsequence found.
		"""

		index1 = 0
		index2 = 0
		lcs = bytearray()
		while index1 < len(self.content1) and index2 < len(self.content2):
			b1 = self.content1[index1]
			b2 = self.content2[index2]
			if b1 == b2:
				lcs.append(b1)
				index1 += 1
				index2 += 1
			else:
				# If the current character of content1 appears ldiffater in content2, advance in content2
				maybeIndex2 = self.content2.find(b1, index2)
				if maybeIndex2 != -1:
					lcs.append(b1)
					index1 += 1
					index2 = maybeIndex2 + 1
				else:
					index1 += 1
		return lcs


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Test the equality of 2 files.")
	parser.add_argument(
	    "file1",
	    type=pathlib.Path,
	    help="The first file to be tested.",
	)
	parser.add_argument(
	    "file2",
	    type=pathlib.Path,
	    help="The second file to be tested.",
	)
	args = parser.parse_args()

	content1 = args.file1.read_bytes()
	content2 = args.file2.read_bytes()

	diff = Diff(content1, content2)

	if diff.isMatch:
		sys.exit(0)

	visitor = VisitorPrint(color=True, width=80, file1=args.file1.as_posix(), file2=args.file2.as_posix())

	diff.visit(visitor.visitBinary)
	sys.exit(1)
