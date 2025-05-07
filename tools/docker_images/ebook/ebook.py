#!/usr/bin/env python3

import argparse
import enum
import glob
import json
import pathlib
import shutil
import subprocess
import tempfile
import typing


class CalibreKey(enum.Enum):
	adobe = enum.auto()


class Calibre:
	uid_ = 0

	def __init__(self, configPath: pathlib.Path) -> None:
		self.configPath = configPath
		assert self.configPath.is_dir(
		), f"Calibre configuration path '{self.configPath}' does not exists or is not a directory."

	@staticmethod
	def getUid() -> int:
		Calibre.uid_ += 1
		return Calibre.uid_

	def autoDetectKeyType(self, path: pathlib.Path) -> typing.Optional[CalibreKey]:
		if "adobe" in str(path).lower():
			return CalibreKey.adobe
		return None

	def addKeyToConfig(self, key: str, value: str, isMap: bool) -> None:
		path = self.configPath / "plugins/dedrm.json"

		# Load the config.
		config = {}
		try:
			config = json.loads(path.read_text())
		except:
			pass

		# Update it.
		if isMap:
			config.setdefault(key, {})
			config[key][str(Calibre.getUid())] = value
		else:
			config.setdefault(key, [])
			config[key].append(value)

		# Save it.
		path.write_text(json.dumps(config))

	def addKey(self, key: pathlib.Path) -> None:
		"""Add a key to calibre config."""

		assert key.is_file(), f"The key file '{key}' does not exists."

		# Try to autodetect the key type.
		keyType = self.autoDetectKeyType(key)
		assert keyType is not None, f"Cannot detect the key type for '{key}'."

		if keyType == CalibreKey.adobe:
			content = "".join([f"{b:02x}" for b in key.read_bytes()])
			self.addKeyToConfig("adeptkeys", content, isMap=True)

	def sanitize(self, ebook: pathlib.Path, output: pathlib.Path) -> None:
		"""Sanitize an ebook.
		
		This for example removes the DRM.
		"""

		assert ebook.is_file(), f"The ebook file '{ebook}' does not exists."

		with tempfile.TemporaryDirectory() as dirname:
			subprocess.run(["calibredb", "add", str(ebook), "--with-library", dirname], check=True)

			pattern = pathlib.Path(dirname) / f"**/*{ebook.suffix}"
			matches = glob.glob(str(pattern), recursive=True)
			assert len(matches) == 1, f"Cannot find the output file from calibredb: {str(matches)}"

			shutil.move(matches[0], output)


class EPub:
	"""Extract information from epub."""

	def __init__(self, ebook: pathlib.Path) -> None:
		self.ebook = ebook

	def process(self) -> None:
		pass


if __name__ == "__main__":
	parser = argparse.ArgumentParser(description="e-book sanitizer.")
	parser.add_argument("--key",
	                    dest="keys",
	                    action="append",
	                    type=pathlib.Path,
	                    default=[],
	                    help="Keys to be added to calibre.")
	parser.add_argument("--calibre-config",
	                    type=pathlib.Path,
	                    default=pathlib.Path.home() / ".config/calibre",
	                    help="Calibre configuration path.")
	parser.add_argument("--output", type=pathlib.Path, default=None, help="The name of the output file.")
	parser.add_argument("--format", type=str, default=None, help="Convert the file into another type.")
	parser.add_argument("ebook", type=pathlib.Path, help="The ebook file to sanitize.")

	args = parser.parse_args()

	calibre = Calibre(configPath=args.calibre_config)

	for key in args.keys:
		calibre.addKey(key)

	outputSuffix = args.ebook.suffix if args.format is None else args.format
	output = (args.ebook.parent /
	          (args.ebook.stem + "_sanitized." + outputSuffix)) if args.output is None else args.output
	#assert args.format is None or outputSuffix == output.suffix, f"The format '{args.format}' does not match the output {output}."

	outputSanitized = output.parent / (output.stem + ".temp" + args.ebook.suffix)

	calibre.sanitize(args.ebook, outputSanitized)

	converter = None
	if args.ebook.suffix.lower() == "epub":
		converter = EPub(outputSanitized)

	assert converter is not None, f"Unsupported format."
	converter.process()
	#calibre.convert(outputSanitized, output)

	# bazel run tools/docker_images/ebook -- --key $(pwd)/temp/Adobe_PrivateLicenseKey--anonymous.der $(pwd)/temp/temp.epub
	# tools/docker_images/ebook/ebook.py --key $(pwd)/temp/Adobe_PrivateLicenseKey--anonymous.der --format pdf $(pwd)/temp/temp.epub
