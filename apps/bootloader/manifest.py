import pathlib
import typing
import json
from contextlib import contextmanager


class Manifest:
	"""Manage the manifest.
	
	A manifest is a json file that looks like this:
	{
		binary: <filename>,  // The filename path to be used for the binary
		stable: <filename>,   // The last known stable binary
		update: {
			last: <filename> // Last filename got from the update
			timestamp: <timestamp> // Last time an update successfully occurred
		}
	}
	"""

	def __init__(self, path: pathlib.Path) -> None:
		self.path = path
		self.path.parent.mkdir(parents=True, exist_ok=True)

	def setBinary(self, uid: str, path: typing.Optional[pathlib.Path]) -> bool:
		"""Set the new binary.

		Returns:
			True if the value is changed, False if the value is the same as before.
		"""

		with self.modify() as data:
			data.setdefault(uid, {})
			previousBinary = data[uid].get("binary", None)
			data[uid]["binary"] = None if path is None else str(path)
			return previousBinary != data[uid]["binary"]

	def setStable(self, uid: str) -> bool:
		"""Mark the current binary as stable.
		
		Returns:
			True if the value is changed, False if the value is the same as before.
		"""

		with self.modify() as data:
			data.setdefault(uid, {})
			maybeBinary = data[uid].get("binary", None)
			previousStableBinary = data[uid].get("stable", None)
			data[uid]["stable"] = maybeBinary
			return previousStableBinary != maybeBinary

	def setUpdate(self, uid: str, path: pathlib.Path, timestamp: int) -> None:
		"""Set the current successful update information."""

		with self.modify() as data:
			data.setdefault(uid, {})
			data[uid]["update"] = {"last": str(path), "timestamp": timestamp}

	def clean(self, uid: str) -> None:
		"""Clean a certain record."""

		with self.modify() as data:
			if uid in data:
				del data[uid]

	def load(self) -> typing.Any:
		"""Load the content of the manifest and return it."""

		if self.path.exists():
			data = self.path.read_text()
			return json.loads(data)
		return {}

	@contextmanager
	def modify(self):
		"""Modify the content of the manifest."""

		data = self.load()
		# Here migrate the config from version A to version B if there is a mismatch.
		if data.get("version", 1) != 1:
			pass
		data["version"] = 1
		yield data
		serialized = json.dumps(data, indent=4)
		self.path.write_text(serialized)

	def getBinary(self, uid: str) -> typing.Optional[pathlib.Path]:
		"""Getter for the current binary."""

		binary = self.load().get(uid, {}).get("binary", None)
		if not binary:
			return None
		return pathlib.Path(binary)

	def getStableBinary(self, uid: str) -> typing.Optional[pathlib.Path]:
		"""Getter for the stable binary."""

		binary = self.load().get(uid, {}).get("stable", None)
		if not binary:
			return None
		return pathlib.Path(binary)

	def getLastUpdate(self, uid: str) -> typing.Optional[pathlib.Path]:
		"""Getter for the path of the last binary acquired from an update."""

		binary = self.load().get(uid, {}).get("update", {}).get("last", None)
		if not binary:
			return None
		return pathlib.Path(binary)
