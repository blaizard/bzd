import typing

from bzd.parser.element import Sequence, ElementBuilder

from tools.bdl.visitor import Visitor, CATEGORY_COMPOSITION, CATEGORY_CONFIG, CATEGORY_NESTED, CATEGORY_GLOBAL
from tools.bdl.entities.all import Expression, Nested, Method, Using, Use, Enum, SymbolType, Namespace
from tools.bdl.entities.builder import NamespaceBuilder
from tools.bdl.visitors.symbol_map import SymbolMap

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

	def registerSymbol(self, entity: SymbolType) -> typing.Optional[str]:

		resolve = self.objectContext.resolve
		resolve &= (self.category != CATEGORY_COMPOSITION) or self.objectContext.composition

		# Resolve the symbol
		if resolve:
			entity.resolve(symbols=self.symbols, namespace=self.namespace)

		# Map an entity only if the name is available.
		if not entity.isName:
			return None

		# Build the symbol name and ensure it is unique
		fqn = SymbolMap.namespaceToFQN(name=entity.name, namespace=self.namespace)

		# Save the serialized payload
		self.symbols.insert(fqn=fqn,
			element=entity.element,
			path=self.objectContext.getSource(),
			category=self.category)

		return fqn

	def visitNestedEntities(self, entity: Nested, result: SymbolList) -> None:
		fqn = self.registerSymbol(entity=entity)

		if fqn and self.objectContext.resolve:
			assert fqn is not None
			# Need to map inheritance members to ths new entity
			for inheritanceType in entity.inheritanceList:
				underlyingType = inheritanceType.getEntityResolved(symbols=self.symbols).underlyingType
				entity.assertTrue(condition=underlyingType is not None, message="Inheritance type was not resolved.")
				assert underlyingType is not None
				# Need to copy all references from underlyingType to new symbols with namespace "fqn"
				self.symbols.insertInheritance(fqn=fqn, fqnInheritance=underlyingType, category=self.category)

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

	def visitNamespace(self, entity: Namespace, result: SymbolList) -> None:
		namespace = []
		for name in entity.nameList:
			namespace.append(name)
			fqn = SymbolMap.namespaceToFQN(namespace=namespace)
			self.symbols.insert(fqn=fqn,
				element=NamespaceBuilder(namespace),
				path=None,
				category=self.category,
				conflicts=True)
