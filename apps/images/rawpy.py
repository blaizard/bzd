import pathlib
import typing

import rawpy
from PIL import Image

from apps.images.converter import Converter, OutputFormat, ConversionResult


class ConverterRawpy(Converter):
	extensions = {'.arw'}
	outputFormats = {OutputFormat.JPEG}

	def tryConvert(self, path: pathlib.Path, outputFormat: OutputFormat, output: pathlib.Path) -> ConversionResult:
		"""Attempt to convert the given file into the given format."""

		with rawpy.imread(path.as_posix()) as raw:
			rgb = raw.postprocess(use_camera_wb=True, no_auto_bright=False, bright=1.0)

		image = Image.fromarray(rgb)
		image.save(
		    output,
		    format="JPEG",
		    quality=95,
		)

		return ConversionResult.completed
