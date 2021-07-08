from pathlib import Path
import typing

from bzd.parser.error import Error
from bzd.parser.element import Element

from tools.bdl.entities.all import Expression, Nested, Method, Using, Enum, Namespace, Use, SymbolType


class ResultType:

	def __init__(self, level: int) -> None:
		self.symbolSet: typing.Set[str] = set()
		self.symbols: typing.List[SymbolType] = []
		self.namespace: typing.Optional[Namespace] = None
		self.useDict: typing.Dict[Path, Use] = {}
		self.ext: typing.Dict[str, typing.Any] = {}
		self.level = level

	def __setitem__(self, key: str, value: typing.Any) -> None:
		self.ext[key] = value

	def __getitem__(self, key: str) -> typing.Any:
		return self.ext[key]

	def __delitem__(self, key: str) -> None:
		del self.ext[key]

	def __contains__(self, key: str) -> bool:
		return key in self.ext

	def update(self, ext: typing.Any) -> None:
		"""
		Set extensions.
		"""
		self.ext.update(ext)

	def registerComposition(self, entity: SymbolType) -> None:
		"""
		Register a composition element.
		"""

		pass

	def registerSymbol(self, entity: SymbolType) -> None:
		"""
		Register a symbol to the result type.
		"""

		self.symbols.append(entity)

		# Register a symbol name only if it has a symbol
		if not entity.isName:
			return

		entity.assertTrue(condition=entity.name not in self.symbolSet,
			message="Conflicting symbol '{}', already defined earlier.".format(entity.name))
		self.symbolSet.add(entity.name)

	def registerNamespace(self, entity: Namespace) -> None:
		"""
		Register a namespace
		"""

		entity.assertTrue(condition=self.namespace is None, message="A namespace has already been defined earlier.")
		entity.assertTrue(condition=self.level == 0, message="Namespace can only be defined at top level.")
		self.namespace = entity

	def registerUse(self, entity: Use) -> None:
		"""
		Register a use specifier
		"""

		entity.assertTrue(condition=entity.path not in self.useDict,
			message="Duplicate path '{}' already used earlier.".format(entity.path))
		self.useDict[entity.path] = entity

	@property
	def isNested(self) -> bool:
		return bool(self.nestedList)

	@property
	def nestedList(self) -> typing.List[Nested]:
		return [symbol for symbol in self.symbols if isinstance(symbol, Nested)]

	@property
	def isExpression(self) -> bool:
		return bool(self.expressionList)

	@property
	def expressionList(self) -> typing.List[Expression]:
		return [symbol for symbol in self.symbols if isinstance(symbol, Expression)]

	@property
	def isMethod(self) -> bool:
		return bool(self.methodList)

	@property
	def methodList(self) -> typing.List[Method]:
		return [symbol for symbol in self.symbols if isinstance(symbol, Method)]

	@property
	def isUsing(self) -> bool:
		return bool(self.usingList)

	@property
	def usingList(self) -> typing.List[Using]:
		return [symbol for symbol in self.symbols if isinstance(symbol, Using)]

	@property
	def isEnum(self) -> bool:
		return bool(self.enumList)

	@property
	def enumList(self) -> typing.List[Enum]:
		return [symbol for symbol in self.symbols if isinstance(symbol, Enum)]

	@property
	def isNamespace(self) -> bool:
		return self.namespace != None

	@property
	def isUse(self) -> bool:
		return bool(self.useList)

	@property
	def useList(self) -> typing.List[Use]:
		return sorted(self.useDict.values(), key=lambda x: x.path)

	def __repr__(self) -> str:
		"""
		Human readable string representation of a result.
		"""
		contentList = []
		for use in self.useDict.values():
			contentList.append(str(use))
		if self.namespace is not None:
			contentList.append(str(self.namespace))
		for symbol in self.symbols:
			contentList.append(str(symbol))

		return "\n".join(contentList)
