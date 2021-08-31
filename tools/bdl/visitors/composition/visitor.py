import typing
import pathlib

from bzd.parser.error import Error

from tools.bdl.visitor import CATEGORY_COMPOSITION
from tools.bdl.visitors.symbol_map import SymbolMap
from tools.bdl.object import Object
from tools.bdl.entities.impl.expression import Expression
from tools.bdl.entities.impl.fragment.fqn import FQN


class Composition:

	def __init__(self, includes: typing.Optional[typing.List[pathlib.Path]] = None) -> None:
		self.includes = [] if includes is None else includes
		self.symbols = SymbolMap()
		self.registry: typing.List[Expression] = []
		self.compositions: typing.List[Expression] = []

	def visit(self, bdl: Object) -> "Composition":

		# Build a master symbol list
		self.symbols.update(bdl.symbols)
		return self

	def resolveDependency(self,
		dependencies: typing.Dict[str, typing.Set[str]],
		fqn: str,
		resolved: typing.List[str],
		unresolved: typing.Optional[typing.Set[str]] = None) -> None:

		if unresolved is None:
			unresolved = set()

		unresolved.add(fqn)
		for dep in dependencies[fqn]:
			if dep not in resolved:
				if dep in unresolved:
					entity1 = self.symbols.getEntityResolved(fqn=fqn).value
					entity2 = self.symbols.getEntityResolved(fqn=dep).value
					message = entity1.error(message="Circular dependency detected between this expression...",
						throw=False)
					message += entity2.error(message="...and this one.", throw=False)
					raise Exception(message)
				self.resolveDependency(dependencies, dep, resolved, unresolved)
		if fqn not in resolved:
			resolved.append(fqn)
		unresolved.remove(fqn)

	def process(self) -> None:

		categories = {CATEGORY_COMPOSITION}

		# Resolve all.
		for fqn, entity in self.symbols.items(categories=categories):

			# Resolve the entities, they must all be expressions
			entity.assertTrue(condition=isinstance(entity, Expression),
				message="Composition only supports 'expression', got '{}' instead.".format(entity.category))
			# Resolve the expression
			entity.resolveMemoized(symbols=self.symbols, namespace=entity.namespace)

		dependencies: typing.Dict[str, typing.Set[str]] = {}
		for fqn, entity in self.symbols.items(categories=categories):

			# Unamed entries should be dealt with later.
			if not entity.isName:
				continue

			# Create the dependency map and keep only dependencies from composition
			dependencies[fqn] = {
				dep
				for dep in entity.dependencies if self.symbols.contains(fqn=dep, categories=categories)
			}

		# Compute the dependency orders and identify circular dependencies.
		orderFQNs: typing.List[str] = []
		for fqn in dependencies.keys():
			self.resolveDependency(dependencies, fqn, orderFQNs)
		self.registry = [self.symbols.getEntityResolved(fqn=fqn).value for fqn in orderFQNs]  # type: ignore

		for fqn, entity in self.symbols.items(categories=categories):
			if entity.isName:
				continue
			self.compositions.append(entity)  # type: ignore
