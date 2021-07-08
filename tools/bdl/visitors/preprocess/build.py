import typing

from bzd.parser.element import Sequence

from tools.bdl.visitor import Visitor
from tools.bdl.entities.all import Expression, Nested, Method, Using, Use, Enum, SymbolType
from tools.bdl.visitors.preprocess.symbol_map import SymbolMap

SymbolList = typing.List[SymbolType]

class Build(Visitor[SymbolList]):

	def __init__(self, objectContext: typing.Any) -> None:
		super().__init__()
		self.objectContext = objectContext
		self.symbols = SymbolMap()

	def getSymbolMap(self) -> SymbolMap:
		return self.symbols

	def visitBegin(self, result: SymbolList) -> SymbolList:
		return []

	def registerComposition(self, sequence: SymbolList, name: typing.Optional[str] = None) -> None:
		"""
		Register a composition sequence. All the elements will added the the global
		composition overview.
		It should contain only expressions.
		"""

		for entity in sequence:
			# The symbol is already registered
			print(self.symbols)

			# TODO SAVE this into a composition list

	def registerSymbol(self, entity: SymbolType) -> None:

		# Map an entity only if the name is available.
		if not entity.isName:
			return

		# Build the symbol name and ensure it is unique
		symbol = SymbolMap.makeFQN(name=entity.name, namespace=self.namespace)

		# Save the serialized payload and convert nested dependencies and make them links
		element = entity.element
		if isinstance(entity, Nested):

			preparedElement = element.copy(ignoreNested=["nested", "config", "composition"])
			for category in ["nested", "config", "composition"]:
				nested = element.getNestedSequence(category)
				if nested:
					references = [
						self.makeEntity(category="reference", attrs={"symbol": element.getAttr("name")})
						for element in nested if element.isAttr("name")
					]
					sequence = Sequence.fromSerialize(references)
					preparedElement.setNestedSequence(category, sequence)

			element = preparedElement

		self.symbols.insert(fqn=symbol, element=element, path=self.objectContext.getSource(), category=self.category)

	def visitNestedEntities(self, entity: Nested, result: SymbolList) -> None:
		if entity.type in ["struct", "interface", "component"]:
			self.registerSymbol(entity=entity)
		elif entity.type == "composition":
			self.registerComposition(sequence=entity.nested, name=entity.name if entity.isName else None)
		else:
			entity.error(message="Unsupported entity type: '{}'.".format(entity.type))

	def visitExpression(self, entity: Expression, result: SymbolList) -> None:
		self.registerSymbol(entity=entity)
		result.append(entity)

	def visitMethod(self, entity: Method, result: SymbolList) -> None:
		self.registerSymbol(entity=entity)

	def visitUsing(self, entity: Using, result: SymbolList) -> None:
		self.registerSymbol(entity=entity)

	def visitEnum(self, entity: Enum, result: SymbolList) -> None:
		self.registerSymbol(entity=entity)

	def visitUse(self, entity: Use, result: SymbolList) -> None:
		if self.objectContext.includeDeps:
			entity.assertTrue(condition=self.objectContext.isPreprocessed(entity.path),
				message="Cannot find preprocessed entity for '{}'.".format(entity.path))

			bdl = self.objectContext.loadPreprocess(path=entity.path)
			self.symbols.update(bdl.symbols)
