import unittest
import pathlib
import tempfile
import typing

from apps.ebook.comics.cbz import Cbz
from apps.ebook.providers import ProviderEbook, ProviderImages


class TestRun(unittest.TestCase):

	def assertProviderEbook(self, provider: ProviderImages, expectedImages: typing.List[str]) -> None:
		self.assertEqual(len(provider.images), len(expectedImages), "There is not the same number of images.")
		for image, expected in zip(provider.images, expectedImages):
			self.assertTrue(str(image).endswith(expected), f"Expected '{expected}' but got '{image}'.")

	def testSample1(self) -> None:
		"""The content look as follow:
		├── file001.jpg
		├── file002.jpg
		├── file003.jpg
		└── other.txt
		"""
		cbz = Cbz()
		provider = ProviderEbook(ebook=pathlib.Path(__file__).parent / "data/sample1.cbz")
		with tempfile.TemporaryDirectory() as directory:
			result = cbz.process(provider=provider, directory=pathlib.Path(directory))
			self.assertEqual(len(result), 1)
			self.assertProviderEbook(result[0][0], expectedImages=[
			    "file001.jpg",
			    "file002.jpg",
			    "file003.jpg",
			])

	def testSample2(self) -> None:
		"""The content look as follow:
		├── nested
		│   ├── file001.jpg
		│   ├── file002.jpg
		│   ├── file003.jpg
		│   └── other.txt
		└── other.txt
		"""
		cbz = Cbz()
		provider = ProviderEbook(ebook=pathlib.Path(__file__).parent / "data/sample2.cbz")
		with tempfile.TemporaryDirectory() as directory:
			result = cbz.process(provider=provider, directory=pathlib.Path(directory))
			self.assertEqual(len(result), 1)
			self.assertProviderEbook(result[0][0],
			                         expectedImages=[
			                             "nested/file001.jpg",
			                             "nested/file002.jpg",
			                             "nested/file003.jpg",
			                         ])

	def testSample3(self) -> None:
		"""The content look as follow:
		├── file001.jpg
		├── file002.jpg
		├── file003.jpg
		├── other.txt
		└── outlier.jpg
		"""
		cbz = Cbz()
		provider = ProviderEbook(ebook=pathlib.Path(__file__).parent / "data/sample3.cbz")
		with tempfile.TemporaryDirectory() as directory:
			result = cbz.process(provider=provider, directory=pathlib.Path(directory))
			self.assertEqual(len(result), 1)
			self.assertProviderEbook(result[0][0], expectedImages=[
			    "file001.jpg",
			    "file002.jpg",
			    "file003.jpg",
			])

	def testSample4(self) -> None:
		"""The content look as follow:
		├── credit.jpg
		├── nested
		│   ├── file001.jpg
		│   ├── file002.jpg
		│   ├── file003.jpg
		│   └── other.txt
		└── other.txt
		"""
		cbz = Cbz()
		provider = ProviderEbook(ebook=pathlib.Path(__file__).parent / "data/sample4.cbz")
		with tempfile.TemporaryDirectory() as directory:
			result = cbz.process(provider=provider, directory=pathlib.Path(directory))
			self.assertEqual(len(result), 2)
			self.assertProviderEbook(result[0][0], expectedImages=["credit.jpg"])
			self.assertProviderEbook(result[1][0],
			                         expectedImages=[
			                             "nested/file001.jpg",
			                             "nested/file002.jpg",
			                             "nested/file003.jpg",
			                         ])

	def testSample5(self) -> None:
		"""The content look as follow:
		└── no_valid_files.txt
		"""
		cbz = Cbz()
		provider = ProviderEbook(ebook=pathlib.Path(__file__).parent / "data/sample5.cbz")
		with tempfile.TemporaryDirectory() as directory:
			result = cbz.process(provider=provider, directory=pathlib.Path(directory))
			self.assertEqual(len(result), 0)

	def testSample6(self) -> None:
		"""The content look as follow:
		├── 1.jpg
		├── 2.jpg
		├── 3.jpg
		└── other.txt
		"""
		cbz = Cbz()
		provider = ProviderEbook(ebook=pathlib.Path(__file__).parent / "data/sample6.cbz")
		with tempfile.TemporaryDirectory() as directory:
			result = cbz.process(provider=provider, directory=pathlib.Path(directory))
			self.assertEqual(len(result), 1)
			self.assertProviderEbook(result[0][0], expectedImages=[
			    "1.jpg",
			    "2.jpg",
			    "3.jpg",
			])

	def testSample7(self) -> None:
		"""The content look as follow:
		├── Thorgal 030 - The Selkie-000.jpg
		├── ...
		"""
		cbz = Cbz()
		provider = ProviderEbook(ebook=pathlib.Path(__file__).parent / "data/sample7.cbz")
		with tempfile.TemporaryDirectory() as directory:
			result = cbz.process(provider=provider, directory=pathlib.Path(directory))
			self.assertEqual(len(result), 1)
			self.assertProviderEbook(result[0][0],
			                         expectedImages=[
			                             "Thorgal 030 - The Selkie-000.jpg",
			                             "Thorgal 030 - The Selkie-001.jpg",
			                             "Thorgal 030 - The Selkie-002.jpg",
			                             "Thorgal 030 - The Selkie-003.jpg",
			                             "Thorgal 030 - The Selkie-004.jpg",
			                             "Thorgal 030 - The Selkie-005.jpg",
			                             "Thorgal 030 - The Selkie-006.jpg",
			                             "Thorgal 030 - The Selkie-007.jpg",
			                             "Thorgal 030 - The Selkie-008.jpg",
			                             "Thorgal 030 - The Selkie-009.jpg",
			                             "Thorgal 030 - The Selkie-010.jpg",
			                             "Thorgal 030 - The Selkie-011.jpg",
			                             "Thorgal 030 - The Selkie-012.jpg",
			                             "Thorgal 030 - The Selkie-013.jpg",
			                             "Thorgal 030 - The Selkie-014.jpg",
			                             "Thorgal 030 - The Selkie-015.jpg",
			                             "Thorgal 030 - The Selkie-016.jpg",
			                             "Thorgal 030 - The Selkie-017.jpg",
			                             "Thorgal 030 - The Selkie-018.jpg",
			                             "Thorgal 030 - The Selkie-019.jpg",
			                             "Thorgal 030 - The Selkie-020.jpg",
			                             "Thorgal 030 - The Selkie-021.jpg",
			                             "Thorgal 030 - The Selkie-022.jpg",
			                             "Thorgal 030 - The Selkie-023.jpg",
			                             "Thorgal 030 - The Selkie-024.jpg",
			                             "Thorgal 030 - The Selkie-025.jpg",
			                             "Thorgal 030 - The Selkie-026.jpg",
			                             "Thorgal 030 - The Selkie-027.jpg",
			                             "Thorgal 030 - The Selkie-028.jpg",
			                             "Thorgal 030 - The Selkie-029.jpg",
			                             "Thorgal 030 - The Selkie-030.jpg",
			                             "Thorgal 030 - The Selkie-031.jpg",
			                             "Thorgal 030 - The Selkie-032.jpg",
			                             "Thorgal 030 - The Selkie-033.jpg",
			                             "Thorgal 030 - The Selkie-034.jpg",
			                             "Thorgal 030 - The Selkie-035.jpg",
			                             "Thorgal 030 - The Selkie-036.jpg",
			                             "Thorgal 030 - The Selkie-037.jpg",
			                             "Thorgal 030 - The Selkie-038.jpg",
			                             "Thorgal 030 - The Selkie-039.jpg",
			                             "Thorgal 030 - The Selkie-040.jpg",
			                             "Thorgal 030 - The Selkie-041.jpg",
			                             "Thorgal 030 - The Selkie-042.jpg",
			                             "Thorgal 030 - The Selkie-043.jpg",
			                             "Thorgal 030 - The Selkie-044.jpg",
			                             "Thorgal 030 - The Selkie-045.jpg",
			                             "Thorgal 030 - The Selkie-046.jpg",
			                             "Thorgal 030 - The Selkie-047.jpg",
			                             "Thorgal 030 - The Selkie-048.jpg",
			                             "Thorgal 030 - The Selkie-049.jpg",
			                             "Thorgal 030 - The Selkie-050.jpg",
			                         ])


if __name__ == "__main__":
	unittest.main()
