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

	def __init__(self, configPath: pathlib.Path) -> None:
		self.configPath = configPath

	_uid = 0

	@staticmethod
	def getUid() -> int:
		Calibre._uid += 1
		return Calibre._uid

	def autoDetectKeyType(self, path: pathlib.Path) -> typing.Optional[CalibreKey]:
		if "adobe" in str(path).lower():
			return CalibreKey.adobe
		return None

	def _addKeyToConfig(self, key: str, value: str, isMap: bool) -> None:
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
			self._addKeyToConfig("adeptkeys", content, isMap=True)

	def sanitize(self, ebook: pathlib.Path, output: pathlib.Path) -> None:
		"""Sanitize an ebook.
		
		This for example removes the DRM.
		"""

		assert self.configPath.is_dir(
		), f"Calibre configuration path '{self.configPath}' does not exists or is not a directory."
		assert ebook.is_file(), f"The ebook file '{ebook}' does not exists."

		with tempfile.TemporaryDirectory() as dirname:
			subprocess.run(["calibredb", "add", str(ebook), "--with-library", dirname], check=True)

			pattern = pathlib.Path(dirname) / f"**/*{ebook.suffix}"
			matches = glob.glob(str(pattern), recursive=True)
			assert len(matches) == 1, f"Cannot find the output file from calibredb: {str(matches)}"

			shutil.move(matches[0], output)
