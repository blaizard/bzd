from pathlib import Path
import typing

from bzd.parser.error import Error
from bzd.parser.element import Element

from tools.bdl.entity.variable import Variable
from tools.bdl.entity.nested import Nested
from tools.bdl.entity.method import Method
from tools.bdl.entity.using import Using
from tools.bdl.entity.enum import Enum
from tools.bdl.entity.namespace import Namespace
from tools.bdl.entity.use import Use

SymbolType = typing.Union[Variable, Nested, Method, Using, Enum]


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

	def registerSymbol(self, element: Element, entity: SymbolType) -> None:
		"""
		Register a symbol to the result type.
		"""

		if entity.name in self.symbolSet:
			Error.handleFromElement(element=element,
				message="Conflicting symbol '{}', already defined earlier.".format(entity.name))
		self.symbolSet.add(entity.name)
		self.symbols.append(entity)

	def registerNamespace(self, element: Element, entity: Namespace) -> None:
		"""
		Register a namespace
		"""

		if self.namespace is not None:
			Error.handleFromElement(element=element,
				message="A namespace has already been defined earlier '{}'.".format(".".join(self.namespace.nameList)))
		if self.level != 0:
			Error.handleFromElement(element=element, message="Namespace can only be defined at top level.")
		self.namespace = entity

	def registerUse(self, element: Element, entity: Use) -> None:
		"""
		Register a use specifier
		"""

		if entity.path in self.useDict:
			Error.handleFromElement(element=element,
				message="Duplicate path '{}' already used earlier.".format(entity.path))
		self.useDict[entity.path] = entity

	@property
	def isNested(self) -> bool:
		return bool(self.nestedList)

	@property
	def nestedList(self) -> typing.List[Nested]:
		return [symbol for symbol in self.symbols if isinstance(symbol, Nested)]

	@property
	def isVariable(self) -> bool:
		return bool(self.variableList)

	@property
	def variableList(self) -> typing.List[Variable]:
		return [symbol for symbol in self.symbols if isinstance(symbol, Variable)]

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
