import os
import json
import re
from typing import Iterable, List, Dict, Optional
from pathlib import Path
from .filter import Filter
from gitignore_parser import parse_gitignore


class Files:

	def __init__(self,
		path: Path,
		include: Optional[List[str]] = None,
		exclude: Optional[List[str]] = None,
		excludeFile: Optional[str] = None,
		useGitignore: bool = False) -> None:
		configRaw = Path(__file__).parent.parent.parent.joinpath(".sanitizer.json").read_text()
		config = json.loads(configRaw)
		self.path = path
		self.workspace = Files._findWorkspace(path)
		self.exclude = Filter(config.get("exclude", []) + ([] if exclude is None else exclude))
		self.excludeFile = excludeFile
		self.excludeFileCache: Dict[Path, Optional[Filter]] = {}
		self.include = Filter(["**"] if include is None else include)
		self.gitignoreMatches = parse_gitignore(self.workspace / ".gitignore") if useGitignore else None

	@staticmethod
	def _findWorkspace(path: Path) -> Path:
		workspace = path
		while True:
			if workspace.joinpath("WORKSPACE").is_file():
				return workspace
			assert workspace != workspace.parent, "Could not find any workspace directory associated with {}".format(
				path)
			workspace = workspace.parent

	def getWorkspace(self) -> Path:
		"""
		Return the root directory of the workspace.
		"""
		return self.workspace

	def isExcluded(self, relativePath: Path) -> bool:
		"""
		Check if a file is excluded from the search.
		"""
		if self.exclude.match(relativePath):
			return True
		if self.excludeFile:
			searchDir = relativePath
			while searchDir != Path("."):
				searchDir = searchDir.parent
				ignoreFilePath = searchDir / self.excludeFile
				if ignoreFilePath not in self.excludeFileCache:
					path = self.workspace / ignoreFilePath
					self.excludeFileCache[ignoreFilePath] = Filter.fromFile(path) if path.is_file() else None
				excludeFilter = self.excludeFileCache[ignoreFilePath]
				if excludeFilter:
					if excludeFilter.match(relativePath.relative_to(searchDir)):
						return True
		return False

	def data(self, relative: bool = False) -> Iterable[Path]:
		"""
		Generator of discovered file path.
		"""
		for (dirpath, dirnames, filenames) in os.walk(self.path):
			# Ignore symlinks and gitignore directories
			if self.gitignoreMatches:
				dirnames[:] = [
					d for d in dirnames if not Path(dirpath).joinpath(d).is_symlink()
					and not self.gitignoreMatches(Path(dirpath).joinpath(d).as_posix())
				]
				filenames[:] = [
					d for d in filenames if not Path(dirpath).joinpath(d).is_symlink()
					and not self.gitignoreMatches(Path(dirpath).joinpath(d).as_posix())
				]
			for filename in filenames:
				path = Path(dirpath).joinpath(filename)
				relativePath = path.relative_to(self.workspace)
				if self.include.match(relativePath):
					if not self.isExcluded(relativePath):
						yield relativePath if relative else path
