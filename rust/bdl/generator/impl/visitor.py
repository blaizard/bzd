import typing
import json
import pathlib

from typing import Any, Dict, Optional

from bzd.template.template import Template
from bdl.object import Object
from bdl.entities.impl.entity import Entity, EntityExpression
from bdl.entities.impl.expression import Expression
from bdl.entities.impl.fragment.symbol import Symbol
from bdl.entities.impl.fragment.fqn import FQN
from bdl.entities.impl.fragment.parameters_resolved import ParametersResolvedItem
from bdl.entities.impl.types import Category as CategoryOriginal
from bdl.entities.impl.method import Method
from bdl.entities.impl.nested import Nested
from bdl.visitors.composition.components import Context, ExpressionEntry
from bdl.visitors.composition.visitor import CompositionView

from rust.bdl.generator.impl.symbol import fqnToCapitalized as fqnToCapitalizedOriginal
from rust.bdl.generator.impl.symbol import symbolRustToStr


class Transform:
	Category = CategoryOriginal

	def __init__(
		self,
		composition: typing.Optional[CompositionView] = None,
		data: Optional[Dict[str, Any]] = None,
	) -> None:
		self.data = data if data else {}
		self.composition = composition

	def fqnToCapitalized(self, fqn: str) -> str:
		return fqnToCapitalizedOriginal(fqn=fqn)

	def toCamelCase(self, string: str) -> str:
		assert len(string), "String cannot be empty."
		return string[0].upper() + string[1:]

	def symbolToStr(self, symbol: Optional[Symbol]) -> str:
		return symbolRustToStr(symbol)

	def symbolNonConstToStr(self, symbol: Optional[Symbol]) -> str:
		return symbolRustToStr(symbol, nonConst=True)

	def symbolReferenceToStr(self, symbol: Optional[Symbol]) -> str:
		return symbolRustToStr(symbol, reference=True)

	def methodReturnToStr(self, method: Method) -> str:
		return f"Result<{self.symbolNonConstToStr(method.symbol)}, bzd::base::error::Error>"

	def methodToStr(self, method: Method) -> str:
		parameters = "".join([f", {item.name}: {self.symbolToStr(item.symbol)}" for item in method.parameters])
		return f"async fn {method.name}(&mut self{parameters}) -> {self.methodReturnToStr(method)};"

	def symbolInterfaceToStr(self, symbol: Optional[Symbol]) -> str:
		assert symbol is not None
		return f"{fqnToCapitalizedOriginal(fqn=symbol.kind)}Interface"

	def inheritanceToStr(self, entity: Nested) -> str:
		if not entity.hasInheritance:
			return ""
		return ": " + " + ".join([self.symbolInterfaceToStr(symbol) for symbol in entity.inheritanceList])

	def isList(self, item: EntityExpression) -> bool:
		return bool(item.symbol.category == CategoryOriginal.builtin and item.symbol.kinds[-1] == "list")

	def listItems(self, item: Expression) -> typing.List[ParametersResolvedItem]:
		return list(item.parametersResolved)

	def listSize(self, item: EntityExpression) -> int:
		return len(self.listItems(typing.cast(Expression, item)))

	def configDependencySymbol(self, item: EntityExpression) -> Optional[Symbol]:
		if item.symbol.category in {CategoryOriginal.interface, CategoryOriginal.component}:
			return item.symbol
		if isinstance(item, Expression) and item.isInterfaceType:
			return item.interfaceType
		if self.isList(item):
			items = self.listItems(typing.cast(Expression, item))
			if items and items[0].param.isSymbol:
				return items[0].param.symbol
		return None

	def configDependencyInterfaceName(self, item: EntityExpression) -> str:
		dependencySymbol = self.configDependencySymbol(item)
		assert dependencySymbol is not None
		return self.symbolInterfaceToStr(dependencySymbol)

	def configTypeItems(self, entity: Entity) -> typing.List[EntityExpression]:
		return [item for item in entity.getConfigAggregated() if self.configDependencySymbol(item) is not None]

	def configTypeParameters(self, entity: Entity) -> str:
		items = self.configTypeItems(entity)
		if not items:
			return ""
		return "<{}>".format(", ".join([f"{self.toCamelCase(item.name)}Type" for item in items]))

	def configTypeArguments(self, entity: Entity) -> str:
		items = self.configTypeItems(entity)
		return "({})".format(
			", ".join([f"{self.toCamelCase(item.name)}Type" for item in items]) + (", " if len(items) == 1 else "")
		)

	def configTypeBounds(self, entity: Entity) -> str:
		items = self.configTypeItems(entity)
		if not items:
			return ""
		return " where {}".format(
			" ".join(
				[f"{self.toCamelCase(item.name)}Type: {self.configDependencyInterfaceName(item)} + 'static," for item in items]
			)
		)

	def configConstructorFields(self, entity: Entity) -> str:
		return ", ".join([item.name for item in entity.getConfigAggregated()])

	def configConstructorValueToStr(self, item: EntityExpression) -> str:
		if self.isList(item):
			return f"[&'static mut {self.toCamelCase(item.name)}Type; {self.listSize(item)}]"
		if self.configDependencySymbol(item) is not None:
			return f"&'static mut {self.toCamelCase(item.name)}Type"
		return symbolRustToStr(item.symbol)

	def configConstructorParameters(self, entity: Entity) -> str:
		return ", ".join([f"{item.name}: {self.configConstructorValueToStr(item)}" for item in entity.getConfigAggregated()])

	def literalNativeToStr(self, literalNative: Any) -> str:
		"""Convert a literal native type into its Rust representation."""

		if isinstance(literalNative, dict):
			assert "type" in literalNative, (
				f"Extended literal must be a dictionary with a field 'type', not: {str(literalNative)}"
			)
			literalType = literalNative["type"]
			if literalType == "enum":
				return fqnToCapitalizedOriginal(literalNative["fqn"])
			raise KeyError(f"Unsupported extended literal of type '{literalType}'")

		if isinstance(literalNative, str):
			return json.dumps(literalNative)

		if isinstance(literalNative, bool):
			return "true" if literalNative else "false"

		return str(literalNative)

	def registryNameToStr(self, fqn: str) -> str:
		return "registry_{}".format("_".join(FQN.toNamespace(fqn)))

	def entryStructNameToStr(self, fqn: str) -> str:
		return "{}Entry".format(self.fqnToCapitalized(fqn))

	def runFunctionNameToStr(self, context: Context) -> str:
		return "run{}".format(self.fqnToCapitalized(context.executorWithoutTarget))

	def isExecutorEntry(self, entity: ExpressionEntry, context: Context) -> bool:
		return entity.expression.fqn == context.executor

	def configDependencyItems(self, entity: ExpressionEntry) -> typing.List[ParametersResolvedItem]:
		"""Get the resolved config items that reference a dependency (component or interface)."""

		return [
			item
			for item in entity.expression.parametersResolved
			if item.param.isSymbol and self.configDependencySymbol(item.param) is not None
		]

	def configParamValueToStr(self, item: ParametersResolvedItem) -> str:
		"""Render the value of a resolved config parameter, either a literal or a registry reference."""

		if item.param.isLiteral:
			return self.literalNativeToStr(item.param.literalNative)
		param = typing.cast(Expression, item.param)
		if param.isSymbol and self.isList(param):
			values = [self.configParamValueToStr(value) for value in self.listItems(param)]
			return "[{}]".format(", ".join(values))
		fqn = param.underlyingValueFQN
		assert fqn is not None, f"The parameter '{param}' must reference a registry entry."
		return "&mut {}().instance".format(self.registryNameToStr(fqn))

	def configParamConcreteType(self, item: ParametersResolvedItem, context: Context) -> Optional[str]:
		"""Resolve the concrete type of a config dependency, through the registry if possible."""

		param = typing.cast(Expression, item.param)
		if param.isSymbol and self.isList(param):
			items = self.listItems(param)
			if items:
				return self.configParamConcreteType(items[0], context=context)
			return None
		if param.isLValue:
			fqn = param.underlyingValueFQN
			assert fqn is not None
			assert self.composition is not None
			entry = self.composition.registry[context].get(fqn)
			if entry is not None:
				return self.symbolToStr(entry.expression.symbol)
		dependencySymbol = self.configDependencySymbol(param)
		return self.symbolToStr(dependencySymbol) if dependencySymbol is not None else None

	def constraintTypesToStr(self, entity: ExpressionEntry, context: Context) -> str:
		"""Generate the constraint types argument, e.g. 'BzdParentContextConstraintTypes<DefaultChild>', or '' if no dependency."""

		items = self.configDependencyItems(entity)
		if not items:
			return ""
		args = []
		for item in items:
			concreteType = self.configParamConcreteType(item, context=context)
			assert concreteType is not None, f"Cannot resolve the concrete type of the dependency '{item}'."
			args.append(concreteType)
		return "{}ContextConstraintTypes<{}>".format(self.symbolToStr(entity.expression.symbol), ", ".join(args))

	def entryTypeToStr(self, entity: ExpressionEntry, context: Context) -> str:
		"""Generate the concrete component type of a registry entry."""

		if self.isExecutorEntry(entity, context):
			return entity.expression.symbol.propertyName
		baseType = self.symbolToStr(entity.expression.symbol)
		constraint = self.constraintTypesToStr(entity, context)
		return "{}<{}>".format(baseType, constraint) if constraint else baseType

	def entryConstructorToStr(self, entity: ExpressionEntry, context: Context) -> str:
		"""Generate the construction call of a registry entry instance."""

		if self.isExecutorEntry(entity, context):
			return "{}::new()".format(entity.expression.symbol.propertyName)
		return "{}::new({})".format(self.entryTypeToStr(entity, context), self.contextNewToStr(entity, context))

	def contextNewToStr(self, entity: ExpressionEntry, context: Context) -> str:
		"""Generate the context construction call of a registry entry."""

		contextType = "{}Context".format(self.symbolToStr(entity.expression.symbol))
		if self.configDependencyItems(entity):
			constraint = self.constraintTypesToStr(entity, context)
			args = ", ".join(self.configParamValueToStr(item) for item in entity.expression.parametersResolved)
			return "{}::<{}>::new({})".format(contextType, constraint, args)
		fields = ", ".join(
			"{}: {}".format(item.name, self.configParamValueToStr(item)) for item in entity.expression.parametersResolved
		)
		return "{} {{ {} }}".format(contextType, fields)

	def initMethodNameToStr(self, expression: Expression) -> str:
		"""Get the method name of an init/shutdown expression, used on the component instance."""

		return expression.symbol.propertyName

	def workloadMethodToStr(self, entry: ExpressionEntry) -> str:
		"""Generate the method call of a workload/service entry on its component instance."""

		symbol = entry.expression.symbol
		assert symbol.this, f"The workload '{entry.expression}' must be a method call on a component instance."
		parameters = ", ".join(self.configParamValueToStr(item) for item in entry.expression.parametersResolved)
		return "{}().instance.{}({})".format(self.registryNameToStr(symbol.this), symbol.propertyName, parameters)


def formatRust(bdl: Object, data: typing.Optional[typing.Dict[str, typing.Any]] = None) -> str:
	template = Template.fromPath(pathlib.Path(__file__).parent / "template/file.rs.btl", indent=True)
	return template.render(bdl.tree, Transform(data=data))


def compositionRust(
	composition: CompositionView,
	data: typing.Optional[typing.Dict[str, typing.Any]] = None,
) -> str:
	template = Template.fromPath(pathlib.Path(__file__).parent / "template/composition.rs.btl", indent=True)
	return template.render(composition, Transform(composition=composition, data=data))
