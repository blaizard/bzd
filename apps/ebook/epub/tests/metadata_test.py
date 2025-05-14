import unittest
import pathlib
import tempfile
import datetime

from apps.ebook.epub.epub import EPub
from apps.ebook.epub.tests.driver_mock import DriverMock
from apps.ebook.epub.metadata import EPubMetadata
from apps.ebook.providers import ProviderEbook, ProviderEbookMetadata


class TestRun(unittest.TestCase):

	def testSample1(self) -> None:
		epub = EPub(driver=DriverMock(expected=[
		    EPubMetadata(document=[pathlib.Path("c001_chap.xhtml"),
		                           pathlib.Path("c002_chap.xhtml")],
		                 metadata=ProviderEbookMetadata(title="Title",
		                                                identifier="urn:uuid:32133-1629-43-ac27-4323234",
		                                                language="fr",
		                                                creators=['creator1', 'creator2', 'creator3'],
		                                                date=datetime.datetime(2023, 1, 1),
		                                                publisher="publisher"))
		]))

		provider = ProviderEbook(ebook=pathlib.Path(__file__).parent / "data/sample1.epub")
		with tempfile.TemporaryDirectory() as directory:
			epub.process(provider=provider, directory=pathlib.Path(directory))


if __name__ == "__main__":
	unittest.main()
