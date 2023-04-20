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
from tools.bdl.visitors.composition.components import Components, ExpressionEntry, EntryType, DependencyGroup, Context


class Entities:
	"""Class to handle entities and their dependencies."""

	defaultExecutorName_ = "~defaultExecutor"

	def __init__(self, symbols: SymbolMap, targets: typing.Set[str]) -> None:
		self.symbols = symbols
		# Map of all available components and their dependencies.
		self.expressions = Components()
		# Map of all available connections.
		self.connections = Connections(self.symbols)
		# The targets of this system.
		self.targets = targets

	def processFirstStage(self, expression: Expression,
	                      fqns: typing.Optional[typing.Set[str]]) -> typing.Optional[Expression]:
		"""Process first stage expression and return the one that are not processed."""

		if expression.isSymbol:
			if expression.symbol.fqn == "bind":

				assert fqns is not None, "FQNs must be set with bind."
				expression.contracts.assertOnly({"executor"})
				for param in expression.parameters:
					identifiers = set(param.regexpr.match(fqns) if param.isRegexpr else [str(param.symbol)])
					for fqn in identifiers:
						entity = self.symbols.getEntityResolved(fqn=fqn).assertValue(element=expression.element)
						entity.contracts.merge(expression.contracts)
				return None
		return expression

	def processMeta(self, expression: Expression) -> None:
		"""Process a meta expression, a special function from the language."""

		expression.assertTrue(condition=expression.isSymbol, message=f"Meta expressions must have a symbol.")
		if expression.symbol.fqn == "connect":
			arguments = []
			for paramResolved in expression.parametersResolved:
				#self.addResolvedDependency(paramResolved.param, executor=None)
				arguments.append(paramResolved.param)

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

	def getContext(self, expression: Expression, defaultExecutor: typing.Optional[str]) -> Context:
		"""Identify the executor and target pair associated with an expression."""

		executor = expression.executor

		if expression.isSymbol:
			if executor is not None:
				if expression.symbol.isThis:
					resolver = expression.makeResolver(symbols=self.symbols)
					this = expression.symbol.getThisResolved(resolver=resolver)
					expression.assertTrue(
					    condition=this.executor == executor,
					    message=
					    f"The executors between this expression and its instance, mismatch: '{executor}' vs '{this.executor}'."
					)
			# If there is a 'this', propagate the executor.
			elif expression.symbol.isThis:
				resolver = expression.makeResolver(symbols=self.symbols)
				this = expression.symbol.getThisResolved(resolver=resolver)
				executor = this.executor

		if executor is None:
			executor = defaultExecutor

		expression.assertTrue(condition=executor is not None,
		                      message="This expression does not have an executor assigned.")

		# Make sure the executor exists.
		assert executor
		entity = self.symbols.getEntityResolved(fqn=executor).assertValue(element=expression.element)

		# Identify the target.
		target = None
		for t in self.targets:
			if executor.startswith(f"{t}."):
				target = t
		entity.assertTrue(condition=target is not None, message="There is no target associated with this executor.")

		assert target
		return Context(executor=executor, target=target)

	def process(self, expressions: typing.Iterable[Expression]) -> None:

		# Create a set of available fqns. This describes only the expressions that have a name,
		# so at declaration time.
		fqns = {expression.fqn for expression in expressions if expression.isFQN and expression.isName}

		# First stage.
		executors: typing.List[Expression] = []
		workloads: typing.List[Expression] = []
		platforms: typing.List[Expression] = []
		meta: typing.List[Expression] = []
		for expression in expressions:
			resolver = expression.makeResolver(symbols=self.symbols)
			expression.resolveMemoized(resolver=resolver)

			if self.processFirstStage(expression=expression, fqns=fqns) is None:
				continue

			# Classify the expressions.
			if expression.isSymbol:
				if expression.isExecutor:
					executors.append(expression)
				elif expression.namespace and expression.namespace[-1] == "platform":
					platforms.append(expression)
				elif expression.isRoleMeta:
					meta.append(expression)
				else:
					underlyingType = expression.getEntityUnderlyingTypeResolved(resolver=resolver)
					if underlyingType.category == Category.method:
						workloads.append(expression)

		# Find the name of the default executor.
		defaultExecutor = executors[0].fqn if len(executors) == 1 else None

		# Second stage.
		# - workloads and executors must have a valid executor.
		for expression in executors + workloads:

			context = self.getContext(expression=expression, defaultExecutor=defaultExecutor)
			resolver = context.makeResolver(symbols=self.symbols, expression=expression)
			expression.resolve(resolver=resolver)
			self.addResolved(expression=expression, context=context)

		# - meta should be processed at the end, to ensure that all symbols are
		# available at that time. Symbols like class instance member for example.
		for expression in meta:
			self.processMeta(expression=expression)

		# Final stage.
		self.close()

	def addResolvedDependency(self, entity: Entity, context: Context) -> None:
		"""Add a new dependency to the composition."""

		entity.assertTrue(condition=isinstance(entity, Expression),
		                  message="Only expressions can be added to the composition.")
		expression = typing.cast(Expression, entity)

		if expression.isFQN:
			self.addResolved(expression=expression, isDepedency=True, context=context)

		else:
			# If the entity does not have a fqn (hence a namespace), process its dependencies.
			# This is the case for parameters for example.
			for fqn in expression.dependencies:
				dependency = self.symbols.getEntityResolved(fqn=fqn).value
				if isinstance(dependency, Expression):
					resolver = context.makeResolver(symbols=self.symbols, expression=dependency)
					dependency.resolveMemoized(resolver=resolver)
					self.addResolvedDependency(dependency, context=context)

	def addResolved(self, expression: Expression, context: Context, isDepedency: bool = False) -> None:
		"""Add a new entity to the composition."""

		if expression.isRoleMeta:
			self.processMeta(expression=expression)
		elif expression.isSymbol:
			self.processEntry(expression=expression, isDepedency=isDepedency, context=context)

	def createEntityNestedComposition(self,
	                                  element: Element,
	                                  this: Expression,
	                                  resolveNamespace: typing.List[str],
	                                  resolver: Resolver,
	                                  name: typing.Optional[str] = None) -> Entity:
		"""Create a new entity for nested compositions.
		
		Args:
			element: The element to be used as a based to create the entity.
			this: The expression used to create the component containing the composition.
			resolveNamespace: The namespace to be used for resolving the new entity.
			resolver: The resolver associated with the creator.
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
		entity.resolveMemoized(resolver=resolver.make(namespace=resolveNamespace, this=this.fqn))

		return entity

	def processEntry(self, expression: Expression, isDepedency: bool, context: Context) -> None:
		"""Resolve the dependencies for a specific expression."""

		resolver = context.makeResolver(symbols=self.symbols, expression=expression)

		# Find the symbol underlying type.
		underlyingType = expression.getEntityUnderlyingTypeResolved(resolver=resolver)

		# Identify the type of entry.
		if underlyingType.category == Category.method:
			entryType = EntryType.service if isDepedency else EntryType.workload
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
		if expression.isExecutor:
			expression.assertTrue(condition=underlyingType.category == Category.component,
			                      message="Executors must be a component.")
			entryType |= EntryType.executor

		# If top level, only process a certain type of entry.
		if not (isDepedency or (EntryType.workload in entryType) or (EntryType.platform in entryType) or
		        (EntryType.executor in entryType)):
			return

		# Register the entry.
		entry = self.expressions.insert(expression=expression, entryType=entryType, context=context)
		if entry is None:
			return

		# Look for all dependencies and add the expression only.
		for fqn in expression.dependencies:
			dep = resolver.getEntityResolved(fqn=fqn).assertValue(element=expression.element)
			if isinstance(dep, Expression):
				dep.resolveMemoized(resolver=resolver.make(expression=dep))
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
					    resolveNamespace=interfaceEntity.namespace,
					    resolver=resolver)
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
				    resolver=resolver,
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
			self.addResolvedDependency(dependency, context=context)

	def close(self) -> None:

		# Close the connection object.
		self.connections.close()

		# Add platform expressions to all executors.
		#for expression in self.platform.values():
		#	for entry in self.expressions.getDependencies(expression):
		#		entry.executors.update(self.executors.keys())

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
