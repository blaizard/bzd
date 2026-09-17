import typing
import pathlib

from typing import Any, Dict, Optional

from bzd.template.template import Template
from bdl.object import Object
from bdl.entities.impl.entity import Entity, EntityExpression
from bdl.entities.impl.expression import Expression
from bdl.entities.impl.fragment.symbol import Symbol
from bdl.entities.impl.fragment.fqn import FQN
from bdl.entities.impl.fragment.parameters import Parameters
from bdl.entities.impl.fragment.parameters_resolved import ParametersResolvedItem
from bdl.entities.impl.types import Category as CategoryOriginal
from bdl.entities.impl.method import Method
from bdl.entities.impl.nested import Nested
from bdl.visitors.composition.components import Context, ExpressionEntry
from bdl.visitors.composition.visitor import CompositionView
from bdl.visitors.symbol_map import Resolver

from rust.bdl.generator.impl.symbol import fqnToCapitalized as fqnToCapitalizedOriginal
from rust.bdl.generator.impl.symbol import symbolRustToStr


class Transform:
	Category = CategoryOriginal

	def __init__(
		self,
		composition: typing.Optional[CompositionView] = None,
		data: Optional[Dict[str, Any]] = None,
		resolver: typing.Optional[Resolver] = None,
	) -> None:
		self.data = data if data else {}
		self.composition = composition
		self.resolver = resolver

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
		return [item for item in self.configValues(entity) if self.configDependencySymbol(item) is not None]

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
		return ", ".join([item.name for item in self.configValues(entity)])

	def configConstructorValueToStr(self, item: EntityExpression) -> str:
		if self.isList(item):
			return f"[&'static mut {self.toCamelCase(item.name)}Type; {self.listSize(item)}]"
		if self.configDependencySymbol(item) is not None:
			return f"&'static mut {self.toCamelCase(item.name)}Type"
		return symbolRustToStr(item.symbol)

	def configConstructorParameters(self, entity: Entity) -> str:
		return ", ".join([f"{item.name}: {self.configConstructorValueToStr(item)}" for item in self.configValues(entity)])

	def configValues(self, entity: Entity) -> Parameters:
		assert self.resolver is not None
		return entity.getConfigValues(resolver=self.resolver)

	def workloadToPath(self, entry: ExpressionEntry) -> str:
		return "::".join(FQN.toNamespace(entry.expression.symbol.kinds[-1]))

	def contextToExecutorPushCalls(self, context: Context) -> str:
		assert self.composition is not None
		entries = []
		entries += [f".push_workload({self.workloadToPath(entry)}())" for entry in self.composition.workloads[context]]
		entries += [f".push_service({self.workloadToPath(entry)}())" for entry in self.composition.services[context]]
		return "".join(entries)


def formatRust(bdl: Object, data: typing.Optional[typing.Dict[str, typing.Any]] = None) -> str:
	template = Template.fromPath(pathlib.Path(__file__).parent / "template/file.rs.btl", indent=True)
	return template.render(bdl.tree, Transform(data=data, resolver=Resolver(symbols=bdl.symbols)))


def compositionRust(
	composition: CompositionView,
	data: typing.Optional[typing.Dict[str, typing.Any]] = None,
) -> str:
	template = Template.fromPath(pathlib.Path(__file__).parent / "template/composition.rs.btl", indent=True)
	return template.render(composition, Transform(composition=composition, data=data))
