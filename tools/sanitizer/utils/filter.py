#!/usr/bin/python

import re
from pathlib import Path


class Filter:

	def __init__(self, filters):
		self.filters = [Filter._compile(filterStr) for filterStr in filters]

	"""
	Compile a filter
	"""

	@staticmethod
	def _compile(filterStr):
		index = 0
		regexpr = r''
		while index < len(filterStr):
			c = filterStr[index]
			if c == "*":
				if index + 1 < len(filterStr) and filterStr[index + 1] == "*":
					regexpr += r'.*'
					index += 1
				else:
					regexpr += r'[^/]*'
			else:
				regexpr += re.escape(c)
			index += 1
		return re.compile(regexpr + r'$')

	"""
	Match a filter with a string
	"""

	def match(self, path: Path) -> bool:
		for regexpr in self.filters:
			if re.match(regexpr, path.as_posix()):
				return True
		return False
