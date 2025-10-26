import pathlib
import unittest


class MyAssertions(unittest.TestCase):

	def runTest(self) -> None:
		pass


def compare(file1: pathlib.Path, file2: pathlib.Path) -> bool:

	content1 = file1.read_text()
	content2 = file2.read_text()

	try:
		MyAssertions().assertEqual(content1, content2)
		return True
	except AssertionError as e:
		print(e)
		return False
