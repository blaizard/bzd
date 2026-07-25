import json
import pathlib
import unittest

from private.diff_test.common import printUnifiedDiff


class MyAssertions(unittest.TestCase):
	def runTest(self) -> None:
		pass


def compare(file1: pathlib.Path, file2: pathlib.Path, color: bool) -> bool:

	content1 = json.loads(file1.read_text())
	content2 = json.loads(file2.read_text())

	try:
		MyAssertions().assertEqual(content1, content2)
		return True
	except AssertionError:
		pass

	lines1 = json.dumps(content1, indent=2, sort_keys=True).splitlines(keepends=True)
	lines2 = json.dumps(content2, indent=2, sort_keys=True).splitlines(keepends=True)

	printUnifiedDiff(lines1, lines2, file1, file2, color)

	return False
