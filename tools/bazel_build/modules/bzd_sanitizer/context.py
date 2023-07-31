import pathlib
import typing
import json


class Context:
	"""Handle and share the current sanitizer context."""

	def __init__(self, workspace: pathlib.Path, fileList: typing.List[str]) -> None:
		self.workspace = workspace
		self.fileList = fileList

	@staticmethod
	def fromFile(path: pathlib.Path) -> "Context":
		data = json.lods(path.read_text())
		return Context(workspace=pathlib.Path(data["workspace"]), fileList=data["fileList"])

	def toFile(self, path: pathlib.Path) -> None:
		data = {"workspace": str(self.workspace), "fileList": self.fileList}
		path.write_text(json.dumps(data))

	def size(self) -> int:
		return len(self.fileList)
