import typing
from functools import cached_property
from collections import OrderedDict

from bzd.parser.element import Element

from tools.bdl.visitor import Group
from tools.bdl.visitors.symbol_map import SymbolMap, Resolver
from tools.bdl.entities.impl.entity import Entity
from tools.bdl.entities.impl.expression import Expression
from tools.bdl.entities.builder import ExpressionBuilder
from tools.bdl.entities.impl.fragment.symbol import Symbol
from tools.bdl.entities.impl.types import Category
from tools.bdl.visitors.composition.connections import Connections
from tools.bdl.visitors.composition.components import Components, ExpressionEntry, EntryType, DependencyGroup


class Entities:
	"""Class to handle entities and their dependencies."""

	defaultExecutorName_ = "~defaultExecutor"

	def __init__(self, symbols: SymbolMap) -> None:
		self.symbols = symbols
		# Map of all available components and their dependencies.
		self.expressions = Components()
		# Map of all available connections.
		self.connections = Connections(self.symbols)

	def processMeta(self, expression: Expression) -> None:
		"""Process a meta expression, a special function from the language."""

		expression.assertTrue(condition=expression.isSymbol, message=f"Meta expressions must have a symbol.")
		if expression.symbol.fqn == "connect":
			self.connections.add(expression.parametersResolved[0].param, expression.parametersResolved[1].param)

			self.add(expression.parametersResolved[0].param, isDep=True)
			self.add(expression.parametersResolved[1].param, isDep=True)

		else:
			expression.error(message="Unsupported meta expression.")

	@cached_property
	def registry(self) -> typing.Dict[str, Expression]:
		"""The registry."""
		registry = OrderedDict()
		for e in self.expressions:
			if EntryType.registry in e.entryType:
				registry[e.expression.fqn] = e.expression
		return registry

	@cached_property
	def platform(self) -> typing.Dict[str, Expression]:
		"""The platform entities."""

		platform = OrderedDict()
		for e in self.expressions:
			if EntryType.platform in e.entryType:
				platform[e.expression.fqn] = e.expression
		return platform

	@cached_property
	def executors(self) -> typing.Dict[str, Expression]:
		"""The executor entities."""

		executors = OrderedDict()
		for e in self.expressions:
			if EntryType.executor in e.entryType:
				executors[e.expression.fqn] = e.expression
		return executors

	@cached_property
	def workloads(self) -> typing.Iterable[ExpressionEntry]:
		"""List all workloads."""
		return [e for e in self.expressions if EntryType.workload in e.entryType]

	@cached_property
	def services(self) -> typing.Iterable[ExpressionEntry]:
		"""List all services."""
		return [e for e in self.expressions if EntryType.service in e.entryType]

	@cached_property
	def registryConnections(self) -> typing.Dict[str, typing.Dict[str, typing.Any]]:

		result: typing.Dict[str, typing.Dict[str, typing.Any]] = {fqn: OrderedDict() for fqn in self.registry.keys()}

		def addEntry(symbol: Symbol, kind: str, input: Symbol) -> None:
			fqn = symbol.this
			name = symbol.propertyName
			symbol.assertTrue(
			    condition=fqn in self.registry,
			    message=
			    f"The instance of one of the connection to '{str(symbol)}', namely '{fqn}', is not present in the registry."
			)
			symbol.assertTrue(condition=name not in result[fqn],
			                  message=f"The property named '{name}' is connected twice.")
			result[fqn][name] = {"type": kind, "input": input}

		for input, group in self.connections.groups.items():
			addEntry(symbol=input, kind="writer", input=input)
			for output, _ in group.outputs.items():
				addEntry(symbol=output, kind="reader", input=input)

		return result

	def getConnectionsByExecutor(self, fqn: str) -> typing.Iterable[typing.Dict[str, typing.Any]]:
		for input, group in self.connections.groups.items():
			isExecutor = fqn in self.expressions.fromSymbol(symbol=input).value.executors
			result: typing.Dict[str, typing.Any] = {
			    "symbol": group.symbol,
			    "input": input,
			    "emitter": isExecutor,
			    "outputs": []
			}
			for output, metadata in group.outputs.items():
				isOutputExecutor = fqn in self.expressions.fromSymbol(symbol=output).value.executors
				if isOutputExecutor:
					result["outputs"].append({"symbol": output, "history": metadata.history})
					isExecutor = True
			if isExecutor:
				yield result

	def getRegistryByExecutor(self, fqn: str) -> typing.Dict[str, Expression]:
		registry = OrderedDict()
		for entry in self.expressions:
			if EntryType.registry in entry.entryType and fqn in entry.executors:
				registry[entry.expression.fqn] = entry.expression
		return registry

	def getWorkloadsByExecutor(self, fqn: str) -> typing.Iterable[Expression]:
		return [entry.expression for entry in self.workloads if fqn in entry.executors]

	def getServicesByExecutor(self, fqn: str) -> typing.Iterable[Expression]:
		return [entry.expression for entry in self.services if fqn in entry.executors]

	def add(self, entity: Entity, isDep: bool = False, executor: typing.Optional[str] = None) -> None:
		"""Add a new entity."""

		entity.assertTrue(condition=isinstance(entity, Expression),
		                  message="Only expressions can be added to the composition.")
		expression = typing.cast(Expression, entity)

		# If the entity does not have a fqn (hence a namespace), process its dependencies.
		if not expression.isFQN:
			for fqn in expression.dependencies:
				dep = self.symbols.getEntityResolved(fqn=fqn).value
				if isinstance(dep, Expression):
					self.add(dep, isDep=True)
			return

		# Resolve the entity against its namespace.
		resolver = self.symbols.makeResolver(namespace=expression.namespace)
		expression.resolveMemoized(resolver=resolver)

		if expression.isRoleMeta:
			self.processMeta(expression=expression)
		elif expression.isSymbol:
			self.processEntry(expression=expression, isDep=isDep, resolver=resolver, executor=executor)

	def createEntityNestedComposition(self,
	                                  element: Element,
	                                  expression: Expression,
	                                  resolveNamespace: typing.List[str],
	                                  name: typing.Optional[str] = None) -> Entity:
		"""Create a new entity for nested compositions.
		
		Args:
			element: The element to be used as a based to create the entity.
			expression: The expression used to create the component containing the composition.
			resolveNamespace: The namespace to be used for resolving the new entity.
			name: The name to give to the new entry.

		Return:
			The newly created entity.
		"""

		expression.assertTrue(
		    condition=expression.isName,
		    message=f"Nested composition must come from a named expression, coming from {expression} instead.")

		# Insert the new entry in the symbol map.
		fqn = self.symbols.insert(name=name,
		                          namespace=expression.namespace + [expression.name],
		                          path=None,
		                          element=element,
		                          group=Group.composition)
		entity = self.symbols.getEntityResolved(fqn=fqn).value
		resolver = self.symbols.makeResolver(namespace=resolveNamespace, this=expression.fqn)
		entity.resolveMemoized(resolver=resolver)

		return entity

	def processEntry(self, expression: Expression, isDep: bool, resolver: Resolver,
	                 executor: typing.Optional[str]) -> None:
		"""Resolve the dependencies for a specific expression."""

		# Find the symbol underlying type.
		underlyingType = expression.getEntityUnderlyingTypeResolved(resolver=resolver)

		# Identify the type of entry.
		if underlyingType.category == Category.method:
			entryType = EntryType.service if isDep else EntryType.workload
		elif underlyingType.category in {Category.component, Category.struct, Category.enum, Category.using}:
			expression.assertTrue(condition=expression.isName,
			                      message="All expressions from the registry must have a name.")
			entryType = EntryType.registry
		elif underlyingType.category in {Category.builtin}:
			# Ignore all builtins as they are expected to be all the time available.
			return
		else:
			expression.error(
			    message=f"Unsupported entry type '{underlyingType.category.value}' within the composition stage.")

		# Check if a platform.
		if expression.namespace and expression.namespace[-1] == "platform":
			entryType |= EntryType.platform
		# Check if an executor.
		if "bzd.platform.Executor" in underlyingType.getParents():
			expression.assertTrue(condition=underlyingType.category == Category.component,
			                      message="Executors must be a component.")
			entryType |= EntryType.executor
			executor = expression.fqn

		# If top level, only process a certain type of entry.
		if not (isDep or (EntryType.workload in entryType) or (EntryType.platform in entryType) or
		        (EntryType.executor in entryType)):
			return

		# Identify the executor of this entity, it is guarantee by the Expression type that there is always an executor.
		if executor is None:
			maybeExecutorFQN = expression.executor
			if maybeExecutorFQN is None:
				executor = self.defaultExecutorName_
			else:
				executor = self.symbols.getEntityResolved(fqn=maybeExecutorFQN).assertValue(
				    element=expression.element).fqn

		# Register the entry.
		entry = self.expressions.insert(expression=expression, entryType=entryType, executor=executor)
		if entry is None:
			return

		# Look for all dependencies and add the expression only.
		for fqn in expression.dependencies:
			dep = self.symbols.getEntityResolved(fqn=fqn).value
			if isinstance(dep, Expression):
				entry.deps.push(dep)

		# Check if there are pre/post-requisites (init/shutdown functions)
		if underlyingType.isInterface:

			def checkIfInitOrShutdown(interfaceEntity: Entity) -> typing.Optional[DependencyGroup]:
				assert entry is not None
				if interfaceEntity.contracts.has("init"):
					return entry.init
				if interfaceEntity.contracts.has("shutdown"):
					return entry.shutdown
				return None

			for interfaceEntity in underlyingType.interface:
				maybeGroup = checkIfInitOrShutdown(interfaceEntity)
				if maybeGroup is not None:

					newEntity = self.createEntityNestedComposition(
					    element=ExpressionBuilder(symbol=f"this.{interfaceEntity.name}"),
					    expression=expression,
					    resolveNamespace=interfaceEntity.namespace)
					assert isinstance(newEntity, Expression)
					maybeGroup.push(newEntity)

		# Check if there are dependent composition from this entity.
		if underlyingType.isComposition:
			for compositionEntity in underlyingType.composition:

				assert isinstance(compositionEntity, Expression), "All composition entities must be an expression."

				# Create a new entity and associate it with its respective objects.
				entityCopied = compositionEntity.copy()
				newEntity = self.createEntityNestedComposition(
				    element=entityCopied.element,
				    expression=expression,
				    resolveNamespace=entityCopied.namespace,
				    name=compositionEntity.name if compositionEntity.isName else None)
				assert isinstance(newEntity, Expression)
				entry.intra.push(newEntity)

		# Add implicit dependencies.
		for dependency in entry.deps + entry.intra:
			self.add(dependency, isDep=True, executor=executor)

	def close(self) -> None:

		# Close the connection object.
		self.connections.close()

		# Add platform expressions to all executors.
		for expression in self.platform.values():
			for entry in self.expressions.getDependencies(expression):
				entry.executors.update(self.executors.keys())

		# Replace the default executor with an actual value if there is only one executor,
		# otherwise, raise an error.
		executors = self.executors.keys()
		for e in self.expressions:
			# We only check cases with a single executor because, other case should be platforms.
			if len(e.executors) == 1 and self.defaultExecutorName_ in e.executors:
				e.expression.assertTrue(
				    condition=len(executors) <= 1,
				    message=
				    f"No executor is assigned to this expression on a multi-executor ({', '.join([*executors])}) composition."
				)
				e.executors = {*executors}

		self.expressions.close()

	def __str__(self) -> str:
		content = ["==== Components ====", str(self.expressions)]
		content += ["==== Connections ===="]
		content += [str("\n".join([f"\t- {c}" for c in str(self.connections).split("\n")]))]

		return "\n".join(content)
