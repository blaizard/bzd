import pathlib
import typing
import tempfile
import time
import os
from PIL import Image
from enum import Enum

from selenium import webdriver
from selenium.webdriver.chrome.options import Options
from selenium.webdriver.chrome.service import Service
from selenium.webdriver.common.by import By
from webdriver_manager.chrome import ChromeDriverManager
from selenium.webdriver.firefox.options import Options as FirefoxOptions

from apps.ebook.epub.metadata import EPubMetadata
from apps.ebook.utils import percentToString


class ScreenshotType(Enum):
	document = 0
	body = 1

	def next(self) -> "ScreenshotType":
		"""Get the next screenshot type in the enumeration and wrap if it reaches the end."""

		members = list(self.__class__)
		index = members.index(self)
		nextIndex = (index + 1) % len(members)
		return members[nextIndex]


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
		self.driver.fullscreen_window()

		return self

	def process(self, documentIndex: int, metadata: EPubMetadata, directory: pathlib.Path,
	            zoom: float) -> typing.List[pathlib.Path]:
		pageIndex = 1
		screenshotType = ScreenshotType.document
		outputs = []
		directory.mkdir(parents=True, exist_ok=True)

		for document in metadata.document:
			self.driver.get(f"file://{document}")
			time.sleep(0.5)

			# Set the zoom level to ensure the text is readable (only if needed)
			# Only do it if we zoom out as it doesn't seem to work for zooming in.
			if zoom > 1:
				self.driver.execute_script(f"document.body.style.zoom = '{zoom}'")

			retry = 0
			while True:

				# Setting the window as small as possible will add scrollbar, which make it possible to detect the document size.
				self.driver.set_window_size(10, 10)

				# We now have the actual size of the document, with the proper ratio.
				width, height = self.driver.execute_script(
				    "return [document.documentElement.scrollWidth, document.documentElement.scrollHeight];")
				self.driver.set_window_size(width, height)

				pagePath = pathlib.Path(directory) / f"{pageIndex:03}.png"
				commonPrefix = os.path.commonprefix([pagePath, document])
				print(
				    f"Saving {document.relative_to(commonPrefix)} -> {pagePath.relative_to(commonPrefix)} ({width}x{height}) ({percentToString(pageIndex / len(metadata.document))})"
				)

				# Take a screenshot depending on the type.
				if screenshotType == ScreenshotType.document:
					self.driver.save_screenshot(pagePath)
				elif screenshotType == ScreenshotType.body:
					element = self.driver.find_element(By.TAG_NAME, "body")
					element.screenshot(pagePath.as_posix())
				else:
					assert False, f"Unsupported screenshot type: {screenshotType}"

				with Image.open(pagePath) as img:
					imageSize = img.size
					if abs(imageSize[0] - width) > 2 or abs(imageSize[1] - height) > 2:
						assert retry < 4, f"Too many retries, aborting."
						retry += 1
						screenshotType = screenshotType.next()
						print(
						    f"-> Size mismatch {imageSize[0]}x{imageSize[1]} (actual) vs {width}x{height} (expected), retrying with {screenshotType}..."
						)
						continue

					# If the zoom is less than 1, we manually reduce the size of the image.
					if zoom < 1:
						newWidth = int(width * zoom)
						newHeight = int(height * zoom)
						print(f"-> Downsizing to ({newWidth}x{newHeight}).")
						img = img.resize((newWidth, newHeight), Image.LANCZOS)  # type: ignore
						img.save(pagePath)
				break

			pageIndex += 1
			outputs.append(pagePath)

		return outputs

	def __exit__(self, *args: typing.Any) -> None:
		self.driver.close()
