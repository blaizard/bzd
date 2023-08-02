import pathlib
import typing
import json
import datetime

from lib.filter import Filter


class _ContextPrintAction:

	def __init__(self, context: "Context", sizeLeft: int) -> None:
		self.context = context
		self.sizeLeft = sizeLeft

	def printStatus(self, isSuccess: bool, elapsedTime: int) -> None:
		status = f"{self.context.color('passed')}PASSED{self.context.color()}" if isSuccess else f"{self.context.color('failed')}FAILED{self.context.color()}"
		statusString = f" {status} ({elapsedTime:.1f}s)"
		print(("{:>" + str(self.sizeLeft + self.context.colorSize()) + "}").format(statusString))


class Context:
	"""Handle and share the current sanitizer context."""

	def __init__(self, workspace: pathlib.Path, fileList: typing.List[str], check: bool, colors: bool) -> None:
		self.workspace = workspace
		self.fileList = fileList
		self.check = check
		self.colors = colors
		self.lineWidth = 80

	@staticmethod
	def fromFile(path: pathlib.Path) -> "Context":
		data = json.loads(path.read_text())
		return Context(workspace=pathlib.Path(data["workspace"]),
		               fileList=data["fileList"],
		               check=data["check"],
		               colors=data["colors"])

	def toFile(self, path: pathlib.Path) -> None:
		data = {"workspace": str(self.workspace), "fileList": self.fileList, "check": self.check, "colors": self.colors}
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
		    "end": "\033[0m"
		}
		return names[name] if self.colors else ""

	def colorSize(self) -> int:
		return len(self.color("section1")) + len(self.color())

	def data(self,
	         endswith: typing.Optional[typing.Sequence[str]] = None,
	         include: typing.Optional[typing.Sequence[str]] = None) -> typing.Iterable[pathlib.Path]:

		endswith = tuple(s.lower() for s in endswith) if endswith else None
		include = Filter(include) if include else None

		for path in self.fileList:

			isIncluded = not (endswith or include)
			isIncluded = isIncluded or (endswith and path.lower().endswith(endswith))
			isIncluded = isIncluded or (include and include.match(path))

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
