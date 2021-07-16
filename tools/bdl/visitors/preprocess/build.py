import typing

from bzd.parser.element import Sequence

from tools.bdl.visitor import Visitor, CATEGORY_COMPOSITION, CATEGORY_CONFIG, CATEGORY_NESTED, CATEGORY_GLOBAL, CATEGORIES
from tools.bdl.entities.all import Expression, Nested, Method, Using, Use, Enum, SymbolType
from tools.bdl.visitors.preprocess.symbol_map import SymbolMap
from tools.bdl.entities.builder import ElementBuilder, SequenceBuilder

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

	def registerSymbol(self, entity: SymbolType) -> None:

		# Map an entity only if the name is available.
		if not entity.isName:
			return

		# Resolve the symbol
		if self.objectContext.resolve:
			entity.resolve(symbols=self.symbols, namespace=self.namespace)

		# Build the symbol name and ensure it is unique
		symbol = SymbolMap.makeFQN(name=entity.name, namespace=self.namespace)

		# Save the serialized payload and convert nested dependencies and make them links
		element = entity.element
		if isinstance(entity, Nested):

			nestedNamespace = self.namespace + [entity.name]
			preparedElement = element.copy(ignoreNested=CATEGORIES)
			for category in CATEGORIES:
				nested = element.getNestedSequence(category)
				if nested:
					sequence = SequenceBuilder()
					for element in nested:
						if element.isAttr("name"):
							sequence.pushBackElement(
								ElementBuilder("reference").addAttr(
								"name", SymbolMap.makeFQN(name=element.getAttr("name").value,
								namespace=nestedNamespace)))
					preparedElement.setNestedSequence(category, sequence)

			element = preparedElement

		self.symbols.insert(fqn=symbol, element=element, path=self.objectContext.getSource(), category=self.category)

	def visitNestedEntities(self, entity: Nested, result: SymbolList) -> None:
		if entity.type in ["struct", "interface", "component"]:
			self.registerSymbol(entity=entity)
		# Composition acts as a namespace, so we don't want to register the symbol.
		elif entity.type == "composition":
			pass
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
		if self.objectContext.resolve:
			entity.assertTrue(condition=self.objectContext.isPreprocessed(entity.path),
				message="Cannot find preprocessed entity for '{}'.".format(entity.path))

			bdl = self.objectContext.loadPreprocess(path=entity.path)
			self.symbols.update(bdl.symbols)
