import typing

from bzd.parser.element import Element, Sequence, ElementBuilder

from tools.bdl.visitor import Visitor, CATEGORY_COMPOSITION, CATEGORY_CONFIG, CATEGORY_NESTED, CATEGORY_GLOBAL
from tools.bdl.entities.all import Expression, Nested, Method, Using, Use, Enum, EntityType, Namespace
from tools.bdl.entities.builder import NamespaceBuilder
from tools.bdl.visitors.symbol_map import SymbolMap
from tools.bdl.visitors.symbol_tree import SymbolTree


class Build(Visitor[None]):

	def __init__(self, objectContext: typing.Any) -> None:
		super().__init__()
		self.objectContext = objectContext
		self.symbols = SymbolMap()
		self.tree = SymbolTree(symbols=self.symbols)

	def getSymbolMap(self) -> SymbolMap:
		return self.symbols

	def getSymbolTree(self) -> SymbolTree:
		return self.tree

	def registerEntity(self, entity: EntityType) -> str:

		resolve = self.objectContext.resolve
		resolve &= (self.category != CATEGORY_COMPOSITION) or self.objectContext.composition

		# Resolve the symbol
		if resolve:
			entity.resolve(symbols=self.symbols, namespace=self.namespace)

		# Build the symbol name or autogenerate it if no name is present.
		fqn: typing.Optional[str] = SymbolMap.namespaceToFQN(name=entity.name,
			namespace=self.namespace) if entity.isName else None

		# Save the serialized payload
		fqn = self.symbols.insert(fqn=fqn,
			element=entity.element,
			path=self.objectContext.getSource(),
			category=self.category)

		# Register only top level entities
		if self.level == 0:
			self.tree.addEntity(entity=entity)

		assert fqn
		return fqn

	def visitNestedEntities(self, entity: Nested, result: None) -> None:
		fqn = self.registerEntity(entity=entity)

		if fqn and self.objectContext.resolve:
			assert fqn is not None
			# Need to map inheritance members to ths new entity
			for inheritanceType in entity.inheritanceList:
				underlyingType = inheritanceType.getEntityResolved(symbols=self.symbols).underlyingType
				entity.assertTrue(condition=underlyingType is not None, message="Inheritance type was not resolved.")
				assert underlyingType is not None
				# Need to copy all references from underlyingType to new symbols with namespace "fqn"
				self.symbols.insertInheritance(fqn=fqn, fqnInheritance=underlyingType, category=self.category)

	def visitExpression(self, entity: Expression, result: None) -> None:
		self.registerEntity(entity=entity)

	def visitMethod(self, entity: Method, result: None) -> None:
		self.registerEntity(entity=entity)

	def visitUsing(self, entity: Using, result: None) -> None:
		self.registerEntity(entity=entity)

	def visitEnum(self, entity: Enum, result: None) -> None:
		self.registerEntity(entity=entity)

	def visitUse(self, entity: Use, result: None) -> None:
		if self.objectContext.resolve:
			entity.assertTrue(condition=self.objectContext.isPreprocessed(entity.path),
				message="Cannot find preprocessed entity for '{}'.".format(entity.path))

			bdl = self.objectContext.loadPreprocess(path=entity.path)
			self.symbols.update(bdl.symbols)

		self.registerEntity(entity=entity)

	def visitNamespace(self, entity: Namespace, result: None) -> None:
		namespace = []
		for name in entity.nameList:
			namespace.append(name)
			fqn = SymbolMap.namespaceToFQN(namespace=namespace)
			self.symbols.insert(fqn=fqn,
				element=NamespaceBuilder(namespace),
				path=None,
				category=self.category,
				conflicts=True)

		self.registerEntity(entity=entity)
