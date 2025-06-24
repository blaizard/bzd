import pathlib
import typing
import tempfile
import time
import os
from PIL import Image

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.chrome.service import Service
from selenium.webdriver.common.by import By
from webdriver_manager.chrome import ChromeDriverManager
from selenium.webdriver.firefox.options import Options as FirefoxOptions

from apps.ebook.epub.metadata import EPubMetadata


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

		return self

	def process(self, documentIndex: int, metadata: EPubMetadata, directory: pathlib.Path,
	            zoom: float) -> typing.List[pathlib.Path]:
		pageIndex = 1
		outputs = []
		directory.mkdir(parents=True, exist_ok=True)

		for document in metadata.document:
			self.driver.get(f"file://{document}")

			# Set the zoom level to ensure the text is readable (only if needed)
			# Only do it if we zoom out as it doesn't seem to work for zooming in.
			if zoom > 1:
				self.driver.execute_script(f"document.body.style.zoom = '{zoom}'")  # type: ignore

			retry = 0
			while True:

				# Setting the window as small as possible will add scrollbar, which make it possible to detect the document size.
				self.driver.set_window_size(10, 10)
				width, height = self.driver.execute_script(  # type: ignore
				    "return [document.documentElement.scrollWidth, document.documentElement.scrollHeight];")
				self.driver.set_window_size(width, height)

				pagePath = pathlib.Path(directory) / f"{pageIndex:03}.png"
				commonPrefix = os.path.commonprefix([pagePath, document])
				print(
				    f"Saving {document.relative_to(commonPrefix)} -> {pagePath.relative_to(commonPrefix)} ({width}x{height})"
				)
				self.driver.save_screenshot(pagePath)

				with Image.open(pagePath) as img:
					imageSize = img.size
					if imageSize[0] != width or imageSize[1] != height:
						assert retry < 2, f"Too many retries, aborting."
						retry += 1
						print(f"-> Size mismatch {imageSize[0]}x{imageSize[1]} vs {width}x{height}, retrying.")
						continue

					# If the zoom is less than 1, we manually reduce the size of the image.
					if zoom < 1:
						newWidth = int(width * zoom)
						newHeight = int(height * zoom)
						print(f"-> Downsizing to ({newWidth}x{newHeight})")
						img = img.resize((newWidth, newHeight), Image.LANCZOS)  # type: ignore
						img.save(pagePath)
				break

			pageIndex += 1
			outputs.append(pagePath)

		return outputs

	def __exit__(self, *args: typing.Any) -> None:
		self.driver.close()
