import pathlib
import typing
import json
import datetime

from bzd_sanitizer.filter import Filter


class _ContextPrintAction:

	def __init__(self, context: "Context", sizeLeft: int) -> None:
		self.context = context
		self.whiteSpaces = sizeLeft - 15

	def printStatus(self, isSuccess: bool, elapsedTime: int) -> None:
		status = (f"{self.context.color('passed')}PASSED{self.context.color()}"
		          if isSuccess else f"{self.context.color('failed')}FAILED{self.context.color()}")
		print(" " * self.whiteSpaces, end="")
		print(f" {status} ({elapsedTime:.1f}s)")


class Context:
	"""Handle and share the current sanitizer context."""

	def __init__(
	    self,
	    workspace: pathlib.Path,
	    fileList: typing.List[str],
	    check: bool,
	    colors: bool,
	) -> None:
		self.workspace = workspace
		self.fileList = fileList
		self.check = check
		self.colors = colors
		self.lineWidth = 80

	@staticmethod
	def fromFile(path: pathlib.Path) -> "Context":
		data = json.loads(path.read_text())
		return Context(
		    workspace=pathlib.Path(data["workspace"]),
		    fileList=data["fileList"],
		    check=data["check"],
		    colors=data["colors"],
		)

	def toFile(self, path: pathlib.Path) -> None:
		data = {
		    "workspace": str(self.workspace),
		    "fileList": self.fileList,
		    "check": self.check,
		    "colors": self.colors,
		}
		path.write_text(json.dumps(data))

	def size(self) -> int:
		return len(self.fileList)

	def color(self, name: str = "end") -> str:
		names = {
		    "section1": "\033[0;33m",
		    "section2": "\033[0;34m",
		    "passed": "\033[0;32m",
		    "failed": "\033[0;31m",
		    "time": "\033[0;30m",
		    "command": "\033[0;33m",
		    "end": "\033[0m",
		}
		return names[name] if self.colors else ""

	def colorSize(self) -> int:
		return len(self.color("section1")) + len(self.color())

	def _excludeFile(
	    self,
	    path: pathlib.Path,
	    excludeFile: str,
	    cache: typing.Dict[pathlib.Path, Filter],
	) -> bool:
		"""Check if a file is excluded from the search."""

		searchDir = path
		while searchDir != pathlib.Path("."):
			searchDir = searchDir.parent
			ignoreFilePath = searchDir / excludeFile
			if ignoreFilePath not in cache:
				ignoreFilePathFullPath = self.workspace / ignoreFilePath
				cache[ignoreFilePath] = (Filter.fromFile(ignoreFilePathFullPath)
				                         if ignoreFilePathFullPath.is_file() else None)
			excludeFilter = cache[ignoreFilePath]
			if excludeFilter:
				if excludeFilter.match(str(path.relative_to(searchDir))):
					return True
		return False

	def data(
	    self,
	    endswith: typing.Optional[typing.Sequence[str]] = None,
	    include: typing.Optional[typing.Sequence[str]] = None,
	    excludeFile: typing.Optional[str] = None,
	) -> typing.Iterable[pathlib.Path]:
		endswith = tuple(s.lower() for s in endswith) if endswith else None
		include = Filter(include) if include else None
		excludeFileCache: typing.Dict[pathlib.Path, Filter] = {}

		for path in self.fileList:
			# Inclusions
			isIncluded = not bool(endswith or include)
			isIncluded = isIncluded or bool(endswith and path.lower().endswith(endswith))
			isIncluded = isIncluded or bool(include and include.match(path))

			# Exclusions
			isIncluded = isIncluded and not (excludeFile
			                                 and self._excludeFile(pathlib.Path(path), excludeFile, excludeFileCache))

			if isIncluded:
				yield pathlib.Path(path)

	def printAction(self, action: str) -> _ContextPrintAction:
		currentTime = datetime.datetime.now().strftime("%H:%M:%S")
		lineString = f"{self.color('time')}[{currentTime}]{self.color()} {action}..."
		print(lineString, end="", flush=True)

		return _ContextPrintAction(self, self.lineWidth - len(lineString) + self.colorSize())

	def printSection(self, title: str, level: int) -> None:
		line = "==" * level
		line += " "
		line += ("{:=<" + str(self.lineWidth - len(line)) + "}").format(title + " ")

		print(self.color("section" + str(level)) + line + self.color())
