import pathlib
import typing
import json


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
		self.data = self.load()

	def setBinary(self, path: typing.Optional[pathlib.Path]) -> None:
		"""Set the new binary."""

		self.data["binary"] = path
		self.save()

	def setStable(self) -> None:
		"""Mark the current binary as stable."""

		# The binary must be set.
		binary = self.data["binary"]
		self.data["stable"] = str(binary)
		self.save()

	def setUnstable(self) -> None:
		"""Mark the current binary as unstable."""

		# The binary must be set.
		binary = self.data["binary"]
		self.data["unstable"] = str(binary)
		self.save()

	def setUpdate(self, path: pathlib.Path, timestamp: int) -> None:
		"""Set the current successful update information."""

		self.data["update"] = {"last": path, "timestamp": timestamp}
		self.save()

	def load(self) -> typing.Any:
		"""Load the content of the manifest and return it."""

		self.path.parent.mkdir(parents=True, exist_ok=True)
		if self.path.exists():
			data = self.path.read_text()
			return json.loads(data)
		return {}

	def save(self) -> None:
		data = json.dumps(self.data, indent=4)
		self.path.write_text(data)

	@property
	def maybeBinary(self) -> typing.Optional[pathlib.Path]:
		"""Getter for the current binary."""

		binary = self.data.get("binary", None)
		if not binary:
			return None
		return binary

	@property
	def maybeStable(self) -> typing.Optional[pathlib.Path]:
		"""Getter for the stable binary."""

		binary = self.data.get("stable", None)
		if not binary:
			return None
		return binary

	@property
	def maybeLastUpdate(self) -> typing.Optional[pathlib.Path]:
		"""Getter for the path of the last binary acquired from an update."""

		binary = self.data.get("update", {}).get("last", None)
		if not binary:
			return None
		return binary
