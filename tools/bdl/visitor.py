import typing
from pathlib import Path

from bzd.parser.visitor import Visitor as VisitorBase
from bzd.parser.error import handleFromElement, assertHasAttr, assertHasSequence
from bzd.parser.element import Sequence, Element

from tools.bdl.entity.variable import Variable
from tools.bdl.entity.nested import Nested
from tools.bdl.entity.method import Method
from tools.bdl.entity.using import Using
from tools.bdl.entity.namespace import Namespace
from tools.bdl.entity.use import Use

SymbolType = typing.Union[Variable, Nested, Method, Using, Namespace, Use]

class ResultType:

	def __init__(self):
		self.symbolSet = set()
		self.symbols: typing.List[SymbolType] = []
		self.ext = {}

	def __setitem__(self, key, value):
		setattr(self, key, value)

	def __getitem__(self, key):
		return getattr(self, key)

	def __delitem__(self, k):
		self[k] = None

	def setExt(self, ext: typing.Any) -> None:
		"""
		Set extensions.
		"""
		self.ext = ext

	def register(self, entity: SymbolType) -> None:
		"""
		Register a symbol to the result type.
		"""

		if entity.symbol in self.symbolSet:
			handleFromElement(element=element, message="Conflicting symbol '{}', already defined earlier.".format(entity.symbol))
		self.symbolSet.add(entity.symbol)
		self.symbols.append(entity)

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
	def isNamespace(self) -> bool:
		return bool(self.namespaceList)

	@property
	def namespaceList(self) -> typing.List[Namespace]:
		return [symbol for symbol in self.symbols if isinstance(symbol, Namespace)]

	@property
	def isUse(self) -> bool:
		return bool(self.useList)

	@property
	def useList(self) -> typing.List[Use]:
		return [symbol for symbol in self.symbols if isinstance(symbol, Use)]

class Visitor(VisitorBase[ResultType, str]):

	nestedKind = None

	def __init__(self) -> None:
		self.level = 0
		self.hasNamespace = False

	def registerSymbol(self, result: typing.Any, entity: SymbolType) -> None:
		if entity.symbol in result["symbolsSet"]:
			handleFromElement(element=element, message="Conflicting symbol '{}', already defined earlier.".format(entity.symbol))
		result["symbolsSet"].add(entity.symbol)
		result["symbols"].append(entity)

	def visitBegin(self, result: typing.Any) -> ResultType:
		return ResultType()

	def visitElement(self, element: Element, result: ResultType) -> ResultType:
		"""
		Main visitor, called each time a new element is discovered.
		"""

		assertHasAttr(element=element, attr="category")

		# Handle class
		if element.getAttr("category").value == "nested":

			assertHasSequence(element=element, sequence="nested")

			self.level += 1

			sequence = element.getNestedSequence("nested")
			assert sequence is not None
			nestedResult = self._visit(sequence)

			self.level -= 1

			entity = self.visitNestedEntities(entity=Nested(element=element, nested=typing.cast(typing.List[typing.Any], nestedResult)))
			result.register(entity=entity)

		# Handle variable
		elif element.getAttr("category").value == "variable":

			entity = self.visitVariable(entity=Variable(element=element))
			result.register(entity=entity)

		# Handle method
		elif element.getAttr("category").value == "method":

			entity = self.visitMethod(entity=Method(element=element))
			result.register(entity=entity)

		# Handle using
		elif element.getAttr("category").value == "using":

			entity = self.visitUsing(entity=Using(element=element))
			result.register(entity=entity)

		# Handle namespace
		elif element.getAttr("category").value == "namespace":

			entity = self.visitNamespace(entity=Namespace(element=element))
			result.register(entity=entity)

		# Handle use
		elif element.getAttr("category").value == "use":

			entity = self.visitUse(entity=Use(element=element))
			result.register(entity=entity)

		# Should never go here
		else:
			raise Exception("Unexpected element category: {}", element.getAttr("category").value)

		return result

	def visitFinal(self, result: ResultType) -> str:
		return str(result)

	def visitComment(self, context: typing.Any, comment: str) -> str:
		"""
		Called when a comment is discovered.
		"""

		return comment

	def visitNestedEntities(self, entity: Nested) -> Nested:
		"""
		Called when discovering a nested entity.
		"""

		return entity

	def visitVariable(self, entity: Variable) -> Variable:
		"""
		Called when discovering a variable.
		"""

		return entity

	def visitMethod(self, entity: Method) -> Method:
		"""
		Called when discovering a method.
		"""

		return entity

	def visitUsing(self, entity: Using) -> Using:
		"""
		Called when discovering a using keyword.
		"""

		return entity

	def visitNamespace(self, entity: Namespace) -> Namespace:
		"""
		Called when discovering a namespace.
		"""

		return entity

	def visitUse(self, entity: Use) -> Use:
		"""
		Called when discovering an use statement.
		"""

		return entity
