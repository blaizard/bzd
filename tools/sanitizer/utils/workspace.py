#!/usr/bin/python

import os
import json
import re
from typing import Iterable
from pathlib import Path
from .filter import Filter


class Files:

	def __init__(self, path: str, include: list = ["**"], exclude: list = []) -> None:
		with open(os.path.join(os.path.dirname(__file__), "..", ".sanitizer.json"), "r") as f:
			config = json.load(f)
		self.path = path
		self.workspace = Files.findWorkspace_(path)
		self.exclude = Filter(config.get("exclude", []) + exclude)
		self.include = Filter(include)

	@staticmethod
	def findWorkspace_(path):
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

	def getWorkspace(self):
		return self.workspace

	def data(self, relative: bool = False) -> Iterable[str]:
		for (dirpath, dirnames, filenames) in os.walk(self.path):
			for filename in filenames:
				path = os.path.join(dirpath, filename)
				relativePath = os.path.relpath(path, self.workspace)
				if not self.exclude.match(relativePath):
					if self.include.match(relativePath):
						yield relativePath if relative else path
