import pathlib
import typing

from PIL import Image, ImageOps, UnidentifiedImageError

from apps.images.converter import Converter, OutputFormat, ConversionResult


class ConverterPillow(Converter):
	extensions = {'.jpg', '.jpeg', '.png', '.bmp', '.gif'}
	outputFormats = {OutputFormat.JPEG}

	def tryConvert(self, path: pathlib.Path, outputFormat: OutputFormat, output: pathlib.Path) -> ConversionResult:
		"""Attempt to convert the given file into the given format."""

		with Image.open(path) as image:

			if image.format in {"JPEG"} and outputFormat in {OutputFormat.JPEG}:
				# No need to perform a conversion for JPEG if the compression is good enough.
				fileSize = path.stat().st_size
				bpp = fileSize * 8 / (image.width * image.height)
				if bpp < 3.0:
					return ConversionResult.skipped

			exif = image.info.get("exif")
			image = ImageOps.exif_transpose(image)
			image = image.resize((image.width, image.height), Image.Resampling.LANCZOS)

			image.save(
			    output,
			    format="JPEG",
			    exif=exif,
			    quality=95,
			)

		return ConversionResult.completed
