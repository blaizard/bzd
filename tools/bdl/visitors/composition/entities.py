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
			self.connections.add(*[paramResolved.param for paramResolved in expression.parametersResolved])

		else:
			expression.error(message="Unsupported meta expression.")

	@property
	def contexts(self) -> typing.Set[Context]:
		"""Get the set of available contexts."""

		return self.expressions.contexts

	@property
	def all(self) -> typing.Dict[Context, typing.List[ExpressionEntry]]:
		"""Get the sorted expression entry list for a specific context."""

		return self.expressions.all

	@cached_property
	def registry(self) -> typing.Dict[Context, typing.Dict[str, ExpressionEntry]]:
		"""Get the sorted registry expression entry list for a specific context."""

		result: typing.Dict[Context, typing.Dict[str, ExpressionEntry]] = {}
		for context, entries in self.all.items():
			result[context] = OrderedDict()
			for entry in entries:
				if entry.isRegistry:
					result[context][entry.expression.fqn] = entry
		return result

	@cached_property
	def registryFQNs(self) -> typing.Set[str]:
		"""Set of all registry FQNs."""

		return {fqn for _, entries in self.registry.items() for fqn in entries.keys()}

	@cached_property
	def workloads(self) -> typing.Dict[Context, typing.List[ExpressionEntry]]:
		"""Get the sorted wokloads expression entry list for a specific context."""

		result: typing.Dict[Context, typing.List[ExpressionEntry]] = {}
		for context, entries in self.all.items():
			result[context] = [entry for entry in entries if entry.isWorkload]
		return result

	@cached_property
	def services(self) -> typing.Dict[Context, typing.List[ExpressionEntry]]:
		"""Get the sorted services expression entry list for a specific context."""

		result: typing.Dict[Context, typing.List[ExpressionEntry]] = {}
		for context, entries in self.all.items():
			result[context] = [entry for entry in entries if entry.isService]
		return result

	@cached_property
	def ios(self) -> typing.Dict[Context, typing.List[typing.Dict[str, typing.Any]]]:
		"""Provide a dictionary grouped by contexts of all connections.
		
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

		result: typing.Dict[Context, typing.List[typing.Dict[str, typing.Any]]] = {}
		for context in self.contexts:
			result[context] = []
			for identifier, metadata in self.connections.endpoints.items():
				if metadata.isSource:
					entry: typing.Dict[str, typing.Any] = {
					    "symbol": metadata.symbol,
					    "identifier": str(identifier),
					    "sinks": []
					}
					isPartOfContext = bool(self.expressions.fromIdentifier(identifier.this, context=context))
					for identifierSink in metadata.connections:
						if bool(self.expressions.fromIdentifier(identifierSink.this, context=context)):
							entry["sinks"].append({"history": self.connections.endpoints[identifierSink].history})
							isPartOfContext = True
					if isPartOfContext:
						result[context].append(entry)

		return result

	@cached_property
	def iosRegistry(self) -> typing.Dict[str, typing.Dict[str, typing.Any]]:
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

		result: typing.Dict[str, typing.Dict[str, typing.Any]] = {fqn: OrderedDict() for fqn in self.registryFQNs}

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

		return self.getContextFromExecutor(executor=executor)

	def getContextFromExecutor(self, executor: str) -> Context:
		"""Get a context from an executor, throw if none is found."""

		# Identify the target.
		target = None
		for t in self.targets:
			if executor.startswith(f"{t}."):
				target = t
		assert target is not None, f"There is no target associated with this executor '{executor}', available targets are: {str(self.targets)}."

		return Context(executor=executor, target=target)

	def process(self, expressions: typing.Iterable[Expression]) -> None:

		# Create a set of available fqns. This describes only the expressions that have a name,
		# so at declaration time.
		fqns = {expression.fqn for expression in expressions if expression.isFQN and expression.isName}

		# First stage.
		executors: typing.List[Expression] = []
		workloads: typing.List[Expression] = []
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

			# Add argument of the meta, like and a source for example that is only instantiated
			# but not 'used' (like no explicit workload is called).
			for paramResolved in expression.parametersResolved:
				context = self.getContext(expression=typing.cast(Expression, paramResolved.param),
				                          defaultExecutor=defaultExecutor)
				self.addResolvedDependency(paramResolved.param, context=context)

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
		self.connections.close()
		self.expressions.close()

	def __str__(self) -> str:
		content = ["==== Components ====", str(self.expressions)]
		content += ["==== Connections ===="]
		content += [str("\n".join([f"\t- {c}" for c in str(self.connections).split("\n")]))]

		return "\n".join(content)
