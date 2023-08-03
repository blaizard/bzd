import re
from pathlib import Path
from typing import Sequence, Pattern


class Filter:

	def __init__(self, filters: Sequence[str]) -> None:
		self.filters = [Filter._compile(filterStr) for filterStr in filters]

	@staticmethod
	def fromFile(path: Path) -> "Filter":
		"""
        Make a filter object from a file.
        The file must contain a filer per line, it also supports comments starting with '#'.
        """
		with path.open() as f:
			filters = [line for line in [line.strip() for line in f] if line and not line.startswith("#")]
		return Filter(filters=filters)

	@staticmethod
	def _compile(filterStr: str) -> Pattern[str]:
		"""
        Compile a filter
        """
		index = 0
		regexpr = r"^"
		while index < len(filterStr):
			c = filterStr[index]
			if c == "*":
				if index + 1 < len(filterStr) and filterStr[index + 1] == "*":
					regexpr += r".*"
					index += 1
				else:
					regexpr += r"[^/]*"
			else:
				regexpr += re.escape(c)
			index += 1
		return re.compile(regexpr + r"$")

	def match(self, path: Path) -> bool:
		"""
        Match a filter with a string
        """
		for regexpr in self.filters:
			if re.match(regexpr, path.as_posix()):
				return True
		return False
