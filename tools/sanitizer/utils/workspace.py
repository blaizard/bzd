import os
import json
import re
from typing import Iterable
from pathlib import Path
from .filter import Filter


class Files:

	def __init__(self, path: str, include: list = ["**"], exclude: list = []) -> None:
		configRaw = Path(__file__).parent.parent.joinpath(".sanitizer.json").read_text()
		config = json.loads(configRaw)
		self.path = path
		self.workspace = Files._findWorkspace(path)
		self.exclude = Filter(config.get("exclude", []) + exclude)
		self.include = Filter(include)

	@staticmethod
	def _findWorkspace(path) -> Path:
		workspace = Path(path)
		while True:
			if workspace.joinpath("WORKSPACE").is_file():
				return workspace
			assert workspace != workspace.parent, "Could not find any workspace directory associated with {}".format(
				path)
			workspace = workspace.parent

	"""
	Return the root directory of the workspace
	"""

	def getWorkspace(self) -> Path:
		return self.workspace

	def data(self, relative: bool = False) -> Iterable[Path]:
		for (dirpath, dirnames, filenames) in os.walk(self.path):
			for filename in filenames:
				path = Path(dirpath).joinpath(filename)
				relativePath = path.relative_to(self.workspace)
				if not self.exclude.match(relativePath):
					if self.include.match(relativePath):
						yield relativePath if relative else path
