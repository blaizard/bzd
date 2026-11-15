import pathlib
import typing

from bzd.parser.fragments import IGNORE_INDEX_VALUE

if typing.TYPE_CHECKING:
	from bzd.parser.element import Element, Sequence

ContextSerialize = dict[str, str | None]
ContextDataType = typing.Union[pathlib.Path, str, "Sequence", "Element"]


class Context:
	def __init__(
		self,
		path: pathlib.Path | None = None,
		content: str | None = None,
		parent: typing.Union["Element", "Sequence"] | None = None,
	) -> None:
		self.data: ContextDataType | None = None
		self.content: str | None = content
		if path is not None:
			self.setPath(path)
		if parent is not None:
			self.setParent(parent)

	@property
	def hasPathOrContent(self) -> bool:
		return self.path is not None or self.content is not None

	@property
	def path(self) -> pathlib.Path | None:
		if isinstance(self.data, pathlib.Path):
			return self.data
		return None

	def setPath(self, path: pathlib.Path) -> None:
		"""
		Set the path.
		"""
		self.data = path

	def setContent(self, content: str) -> None:
		"""
		Set the content.
		"""
		assert self.content is None
		self.content = content

	@property
	def parent(self) -> typing.Any | None:
		if not self.hasPathOrContent and self.data is not None:
			return self.data
		return None

	def setParent(self, parent: typing.Union["Element", "Sequence"]) -> None:
		"""
		Set the parent.
		"""
		self.data = parent

	def clear(self) -> None:
		self.data = None
		self.content = None

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

	def resolve(
		self,
		element: typing.Optional["Element"] = None,
		attr: str | None = None,
	) -> tuple["Context", int, int]:
		"""
		Get the root context and indexes associated with this context, go up the parent chain if needed.
		"""
		start = IGNORE_INDEX_VALUE
		end = 0
		context = self

		from bzd.parser.element import Element

		previous: set[Context] = set()
		while context is not None:
			if start == IGNORE_INDEX_VALUE and isinstance(element, Element):
				start, end = element.getIndexes(attr=attr)
			attr = None
			if context.parent is None:
				break
			element = context.parent
			assert element
			context = element.context
			assert context not in previous, "Infinite loop between context."
			previous.add(context)

		return context, start, end

	def __repr__(self) -> str:
		content: list[str] = []
		if self.path is not None:
			content.append(self.path.as_posix())
		if self.content is not None:
			content.append("<content>")
		if self.parent is not None:
			content.append("<parent>")
		return ",".join(content) if content else "<empty>"
