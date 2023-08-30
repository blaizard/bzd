import re
import typing
import pathlib


class Filter:

	def __init__(self, filters: typing.Sequence[str]) -> None:
		self.filters = [Filter._compile(filterStr) for filterStr in filters]

	@staticmethod
	def fromFile(path: pathlib.Path) -> "Filter":
		"""Make a filter object from a file.
        The file must contain a filer per line, it also supports comments starting with '#'.
        """

		with path.open() as f:
			filters = [line for line in [line.strip() for line in f] if line and not line.startswith("#")]
		return Filter(filters=filters)

	@staticmethod
	def _compile(filterStr: str) -> typing.Pattern[str]:
		"""Compile a filter.

        - `*`: It will match everything except slashes.
        - `**`: Recursively matches zero or more directories that fall under the current directory.
        - `?`: The question mark wildcard is used to match any single character.
        """

		index = 0
		regexpr = r"^"
		while index < len(filterStr):
			c = filterStr[index]
			if c == "*":
				if index + 1 < len(filterStr) and filterStr[index + 1] == "*":
					regexpr += r"(?:.*(?=/)|.*/|)"
					index += 1
				else:
					regexpr += r"[^/]*"
			elif c == "?":
				regexpr += r"."
			else:
				regexpr += re.escape(c)
			index += 1
		return re.compile(regexpr + r"$", re.IGNORECASE)

	def match(self, path: str) -> bool:
		"""Match a filter with a string."""

		return any(re.match(regexpr, path) for regexpr in self.filters)
