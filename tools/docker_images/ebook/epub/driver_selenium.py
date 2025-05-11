import pathlib
import typing
import tempfile
import time
from PIL import Image

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.chrome.service import Service
from selenium.webdriver.common.by import By
from webdriver_manager.chrome import ChromeDriverManager
from selenium.webdriver.firefox.options import Options as FirefoxOptions

from tools.docker_images.ebook.epub.metadata import EPubMetadata


class DriverSelenium:
	"""Selenium driver to extract pages."""

	def __init__(self) -> None:
		pass

	def __enter__(self) -> "DriverSelenium":
		options = FirefoxOptions()
		options.add_argument("--headless")
		# --kiosk is needed to get the same size with the screenshot
		# https://github.com/mozilla/geckodriver/issues/1744
		options.add_argument("--kiosk")
		self.driver = webdriver.Firefox(options=options)

		#self.driver = webdriver.Chrome()
		return self

	def process(self, documentIndex: int, metadata: EPubMetadata, directory: pathlib.Path) -> typing.List[pathlib.Path]:
		pageIndex = 1
		outputs = []
		directory.mkdir(parents=True, exist_ok=True)

		for document in metadata.document:
			self.driver.get(f"file://{document}")

			retry = 0
			while True:

				# Setting the window as small as possible will add scrollbar, which make it possible to detect the document size.
				self.driver.set_window_size(10, 10)
				width, height = self.driver.execute_script(  # type: ignore
				    "return [document.documentElement.scrollWidth, document.documentElement.scrollHeight];")
				self.driver.set_window_size(width, height)

				pagePath = pathlib.Path(directory) / f"{pageIndex:03}.png"
				print(f"Saving {document} -> {pagePath} ({width}x{height})")
				self.driver.save_screenshot(pagePath)

				imageSize = Image.open(pagePath).size
				if imageSize[0] != width or imageSize[1] != height:
					assert retry < 2, f"Too many retries, aborting."
					retry += 1
					print(f"Size mismatch {imageSize[0]}x{imageSize[1]} vs {width}x{height}, retrying.")
				break

			pageIndex += 1
			outputs.append(pagePath)

		return outputs

	def __exit__(self, *args: typing.Any) -> None:
		self.driver.close()
