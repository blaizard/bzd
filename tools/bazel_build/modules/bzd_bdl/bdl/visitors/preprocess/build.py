import typing

from bzd.parser.element import Element, Sequence, ElementBuilder

from bdl.visitor import Visitor, Group
from bdl.entities.all import (
    Expression,
    Nested,
    Method,
    Using,
    Use,
    Enum,
    EnumValue,
    Extern,
    EntityType,
    Namespace,
)
from bdl.entities.builder import NamespaceBuilder
from bdl.visitors.symbol_map import SymbolMap
from bdl.visitors.symbol_tree import SymbolTree


class Build(Visitor[None]):

	def __init__(
	    self,
	    objectContext: typing.Any,
	    namespace: typing.Optional[typing.List[str]] = None,
	) -> None:
		super().__init__(namespace=namespace)
		self.objectContext = objectContext
		self.symbols = SymbolMap()
		self.tree = SymbolTree(symbols=self.symbols)

		# Visit the namespace
		if namespace:
			self.symbols.insert(
			    name=namespace[-1],
			    namespace=namespace[:-1],
			    element=NamespaceBuilder(namespace),
			    path=None,
			    group=self.group,
			    conflicts=True,
			)

	def visitFinal(self, result: None) -> None:
		self.symbols.close()

	def getSymbolMap(self) -> SymbolMap:
		return self.symbols

	def getSymbolTree(self) -> SymbolTree:
		return self.tree

	def registerEntity(self, entity: EntityType) -> str:
		resolve = self.objectContext.resolve
		resolve &= (Group.composition not in self.group) or self.objectContext.composition

		# Save the serialized payload
		fqn = self.symbols.insert(
		    name=entity.name if entity.isName else None,
		    namespace=self.namespace,
		    element=entity.element,
		    path=self.objectContext.getSource(),
		    group=self.group,
		)

		# Resolve the symbol
		if resolve:
			entity.resolveMemoized(resolver=self.symbols.makeResolver(namespace=self.namespace, memoize=True))

		# Register only top level entities
		if self.level == 0:
			self.tree.addEntity(entity=entity)

		assert fqn
		return fqn

	def visitNestedEntities(self, entity: Nested, result: None) -> None:
		fqn = self.registerEntity(entity=entity)

	def visitExpression(self, entity: Expression, result: None) -> None:
		self.registerEntity(entity=entity)

	def visitMethod(self, entity: Method, result: None) -> None:
		self.registerEntity(entity=entity)

	def visitUsing(self, entity: Using, result: None) -> None:
		self.registerEntity(entity=entity)

	def visitExtern(self, entity: Extern, result: None) -> None:
		self.registerEntity(entity=entity)

	def visitEnum(self, entity: Enum, result: None) -> None:
		self.registerEntity(entity=entity)

	def visitEnumValue(self, entity: EnumValue, result: None) -> None:
		self.registerEntity(entity=entity)

	def visitUse(self, entity: Use, result: None) -> None:
		if self.objectContext.resolve:
			maybePreprocess = self.objectContext.findPreprocess(source=entity.path.as_posix())
			entity.assertTrue(
			    condition=maybePreprocess is not None,
			    message=f"Cannot find preprocessed entity for '{entity.path}'.",
			)
			bdl = self.objectContext.loadPreprocess(preprocess=maybePreprocess)
			self.symbols.update(bdl.symbols)

		self.registerEntity(entity=entity)

	def visitNamespace(self, entity: Namespace, result: None) -> None:
		namespace = []
		for name in entity.nameList:
			namespace.append(name)
			self.symbols.insert(
			    name=namespace[-1],
			    namespace=namespace[:-1],
			    element=NamespaceBuilder(namespace),
			    path=None,
			    group=self.group,
			    conflicts=True,
			)

		self.registerEntity(entity=entity)
