import argparse
import os
import json
import pathlib
import typing
from functools import cached_property
import subprocess
import shutil
import datetime

from apps.images.pillow import ConverterPillow
from apps.images.rawpy import ConverterRawpy
from apps.images.converter import OutputFormat, ConversionResult, Converter


class Converters:

	def __init__(self, converters: typing.Sequence[Converter]) -> None:
		self.converters = converters

	@cached_property
	def allExtensions(self) -> typing.Set[str]:
		return {value for converter in self.converters for value in converter.extensions}

	def process(self, path: pathlib.Path, outputFormat: OutputFormat) -> None:
		if args.path.is_file():
			self._processFile(path, outputFormat=outputFormat)
		else:
			for root, dirs, files in args.path.walk():
				for name in sorted(files):
					self._processFile(root / name, outputFormat=outputFormat)

	def _getOutput(self, path: pathlib.Path, outputFormat: OutputFormat) -> typing.Optional[pathlib.Path]:

		if outputFormat == OutputFormat.JPEG:
			output = path.with_suffix(".jpg")

		else:
			assert False, f"Unsupported output format '{outputFormat}'."

		# This makes sure the file is unique.
		outputTemporary = output
		while outputTemporary.exists():
			outputTemporary = outputTemporary.with_stem(f"{outputTemporary.stem}.temp")

		return output, outputTemporary

	def _sizeToString(self, path: pathlib.Path) -> str:
		fileSize = path.stat().st_size
		return f"{fileSize / (1024 * 1024):.2f}MB"

	def _run(self, commands: typing.Sequence[str]) -> bool:
		result = subprocess.run(commands, text=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
		if result.returncode != 0:
			print(f"Command {' '.join(commands)} failed:", result.stdout)
			return False
		return result.stdout

	def _mergeExif(self, path: pathlib.Path) -> None:
		"""Read and merge sidecar exif attributes to the image."""

		processed = []
		json = {}
		for extensions in {".xmp", ".XMP"}:
			file = path.with_name(path.name + extensions)
			if not file.is_file():
				continue
			if self._run([
			    "exiftool", "-P", "-overwrite_original", "-TagsFromFile",
			    file.as_posix(), "-AllDates<DateTimeOriginal", "-DateCreated<DateTimeOriginal",
			    "-FileCreateDate<DateTimeOriginal", "-FileModifyDate<DateTimeOriginal", "-GPS:all<XMP-exif:all",
			    "-GPS:GPSLatitudeRef<Composite:GPSLatitudeRef", "-GPS:GPSLongitudeRef<Composite:GPSLongitudeRef",
			    "-GPS:GPSDateStamp<XMP-exif:GPSDateTime", "-GPS:GPSTimeStamp<XMP-exif:GPSDateTime",
			    path.as_posix()
			]):
				file.unlink()

	def _updateDate(self, path: pathlib.Path) -> None:
		"""Read and merge sidecar exif attributes to the image."""

		output = self._run(["exiftool", "-json", path.as_posix()])
		if not output:
			return

		tz = datetime.timezone.utc
		allDates = [
		    datetime.datetime.fromtimestamp(path.stat().st_atime, tz=tz),
		    datetime.datetime.fromtimestamp(path.stat().st_mtime, tz=tz),
		]
		for data in json.loads(output):
			for key, value in data.items():
				if "date" in key.lower():
					for fmt in ["%Y:%m:%d %H:%M:%S%z", "%Y:%m:%d %H:%M:%S", "%Y:%m:%d"]:
						try:
							dt = datetime.datetime.strptime(value, fmt)
							if dt.tzinfo is None:
								dt = dt.replace(tzinfo=tz)
							if dt > datetime.datetime(1985, 1, 1, tzinfo=tz):
								allDates.append(dt)
							break
						except ValueError:
							continue
		oldestDate = min(allDates)
		oldestTimestamp = int(oldestDate.timestamp())
		os.utime(path, (oldestTimestamp, oldestTimestamp))

	def _moveFiles(self, original: pathlib.Path, file: pathlib.Path, output: pathlib.Path) -> typing.List[pathlib.Path]:
		"""This function moves the files including their sidecar."""

		originalTimes = (original.stat().st_atime, original.stat().st_mtime)

		for extensions in {".xmp", ".XMP"}:
			sidecar = original.with_name(original.name + extensions)
			if not sidecar.is_file():
				continue
			sidecar.rename(output.with_name(output.name + extensions))

		# Copy over EXIF tags properly. This is needed because some tags are lost during the conversion, and exiftool can only copy them from a file with a valid EXIF structure.
		self._run(["exiftool", "-P", "-overwrite_original", "-all=", "-tagsfromfile", "@", "-all:all", file.as_posix()])
		self._run([
		    "exiftool", "-P", "-overwrite_original", "-all:all", "-tagsfromfile",
		    original.as_posix(), "-all:all",
		    file.as_posix()
		])

		original.unlink()
		file.rename(output)
		os.utime(output, originalTimes)

	def _processFile(self, path: pathlib.Path, outputFormat: OutputFormat) -> typing.Optional[pathlib.Path]:
		"""Process a single file."""

		path = path.resolve().absolute()
		extension = path.suffix.lower()

		output, outputTemporary = self._getOutput(path, outputFormat)

		for converter in self.converters:
			if extension not in converter.extensions:
				continue
			print(f"Converting {path} ({self._sizeToString(path)}) with {converter.__class__.__name__}...")

			try:
				result = converter.tryConvert(path=path, outputFormat=OutputFormat.JPEG, output=outputTemporary)

			except Exception as e:
				print(f" -> FAILED: {str(e)}")

			else:
				if result == ConversionResult.skipped:
					shutil.copyfile(path, outputTemporary)

				if result in {ConversionResult.completed, ConversionResult.skipped}:
					assert outputTemporary.is_file(), f"The output '{outputTemporary}' was not created."
					self._moveFiles(original=path, file=outputTemporary, output=output)
					self._mergeExif(path=output)
					self._updateDate(path=output)
					print(f" -> {output} ({self._sizeToString(output)})")
					return output

				else:
					assert False, f"Unsupported conversion result: {result}"

		return None


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="Process image files in bulk.")
	parser.add_argument("--format", default="jpeg", type=OutputFormat, help="The format type.")
	parser.add_argument("path", type=pathlib.Path, help="File or directory to process.")

	args = parser.parse_args()

	converters = Converters(converters=[ConverterPillow(), ConverterRawpy()])
	converters.process(args.path, outputFormat=args.format)
