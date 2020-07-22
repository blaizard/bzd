#!/usr/bin/python

import os
import json
import re
from typing import Iterable
from .filter import Filter


class Files:

	def __init__(self, workspace: str, include: list = ["**"], exclude: list = []) -> None:
		with open(os.path.join(os.path.dirname(__file__), "..", ".sanitizer.json"), "r") as f:
			config = json.load(f)
		self.workspace = workspace
		self.exclude = Filter(config.get("exclude", []) + exclude)
		self.include = Filter(include)

	def data(self, relative: bool = False) -> Iterable[str]:
		for (dirpath, dirnames, filenames) in os.walk(self.workspace):
			for filename in filenames:
				path = os.path.join(dirpath, filename)
				relativePath = os.path.relpath(path, self.workspace)
				if not self.exclude.match(relativePath):
					if self.include.match(relativePath):
						yield relativePath if relative else path
