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
from tools.bdl.visitors.composition.connections import Connections, EndpointId
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
			arguments = []
			for argument in expression.parametersResolved:
				self.add(argument.param, isDep=True)
				arguments.append(argument.param)

			self.connections.add(*arguments)

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
		"""Provide a registry that contains all available connections.
		
		{
			"example.hello": {
				"send": {
					"type": "source",
					"multi": False,
					"connections": {"example.hello.send"},
					"symbol": <symbol>,
					"identifier": "example.hello.send"
				}
			},
			...
		}
		"""

		result: typing.Dict[str, typing.Dict[str, typing.Any]] = {fqn: OrderedDict() for fqn in self.registry.keys()}

		for identifier, metadata in self.connections.endpoints.items():
			if identifier.this in result:
				result[identifier.this][identifier.name] = {
				    "type": "source" if metadata.isSource else "sink",
				    "multi": metadata.multi,
				    "connections": [str(connection) for connection in metadata.connections],
				    "symbol": metadata.symbol,
				    "identifier": str(identifier)
				}

		return result

	def getConnectionsByExecutor(self, fqn: str) -> typing.Iterable[typing.Dict[str, typing.Any]]:
		"""Provide an iterator over all connections sources for a specific executor.
		
		[
			{
				"symbol": <Data type symbol>,
				"source": "example.hello.send",
				"sinks": [
					{
						"history": 1
					},
					...
				]
			},
			...
		]
		"""

		for identifier, metadata in self.connections.endpoints.items():
			if metadata.isSource:
				result: typing.Dict[str, typing.Any] = {
				    "symbol": metadata.symbol,
				    "identifier": str(identifier),
				    "sinks": []
				}
				isExecutor = fqn in self.expressions.fromIdentifier(identifier.this).value.executors
				for identifierSink in metadata.connections:
					if fqn in self.expressions.fromIdentifier(identifierSink.this).value.executors:
						result["sinks"].append({"history": self.connections.endpoints[identifierSink].history})
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
	                                  this: Expression,
	                                  resolveNamespace: typing.List[str],
	                                  name: typing.Optional[str] = None) -> Entity:
		"""Create a new entity for nested compositions.
		
		Args:
			element: The element to be used as a based to create the entity.
			this: The expression used to create the component containing the composition.
			resolveNamespace: The namespace to be used for resolving the new entity.
			name: The name to give to the new entry.

		Return:
			The newly created entity.
		"""

		this.assertTrue(condition=this.isName,
		                message=f"Nested composition must come from a named expression, coming from {this} instead.")

		# Insert the new entry in the symbol map.
		fqn = self.symbols.insert(name=name,
		                          namespace=this.namespace + [this.name],
		                          path=None,
		                          element=element,
		                          group=Group.composition)
		entity = self.symbols.getEntityResolved(fqn=fqn).value

		resolver = self.symbols.makeResolver(namespace=resolveNamespace, this=this.fqn)
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
					    this=expression,
					    resolveNamespace=interfaceEntity.namespace)
					assert isinstance(newEntity, Expression)
					maybeGroup.push(newEntity)

		# Check if there are dependent composition from this entity.
		if underlyingType.isComposition:
			for compositionEntity in underlyingType.composition:

				assert isinstance(compositionEntity, Expression), "All composition entities must be an expression."

				# Create a new entity and associate it with its respective objects.
				entityCopied = compositionEntity.deepCopy()
				newEntity = self.createEntityNestedComposition(
				    element=entityCopied.element,
				    this=expression,
				    resolveNamespace=entityCopied.namespace,
				    name=compositionEntity.name if compositionEntity.isName else None)
				assert isinstance(newEntity, Expression)
				entry.intra.push(newEntity)

		# Add inputs/outputs to the connection list.
		if underlyingType.category in {Category.component}:
			result = self.symbols.getChildren(fqn=expression.fqn, groups={Group.interface})
			for fqn, entity in result.value.items():
				if isinstance(entity, Expression) and entity.isSymbol:
					self.connections.addEndpoint(this=expression, endpoint=entity)

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
