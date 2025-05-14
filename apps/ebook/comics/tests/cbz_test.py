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
		provider = ProviderEbook(ebook=pathlib.Path(__file__).parent / "data/sample1.cbz")
		with tempfile.TemporaryDirectory() as directory:
			result = cbz.process(provider=provider, directory=pathlib.Path(directory))
			self.assertEqual(len(result), 1)
			self.assertProviderEbook(result[0][0], expectedImages=[
			    "1.jpg",
			    "2.jpg",
			    "3.jpg",
			])


if __name__ == "__main__":
	unittest.main()
