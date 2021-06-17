import pathlib
import typing

ContextSerialize = typing.Dict[str, typing.Optional[str]]


class Context:

	def __init__(self, path: typing.Optional[pathlib.Path] = None, content: typing.Optional[str] = None) -> None:
		self.path = path
		self.content = content

	def serialize(self) -> ContextSerialize:
		"""
		Serialize a context.
		"""
		return {"path": None if self.path is None else self.path.as_posix()}

	@staticmethod
	def fromSerialize(context: ContextSerialize) -> "Context":
		"""
		Create a context from a serialized context.
		"""
		path = context["path"]
		return Context(path=None if path is None else pathlib.Path(path))
