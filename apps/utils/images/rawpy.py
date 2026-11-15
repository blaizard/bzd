import pathlib
import typing

import rawpy
from apps.utils.images.converter import ConversionResult, Converter, OutputFormat
from PIL import Image


class ConverterRawpy(Converter):
	extensions: typing.ClassVar[set[str]] = {".arw"}
	outputFormats: typing.ClassVar[set[OutputFormat]] = {OutputFormat.JPEG}

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
