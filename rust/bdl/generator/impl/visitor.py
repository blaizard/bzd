import typing
import pathlib

from typing import Any, Dict, Optional

from bzd.template.template import Template
from bdl.object import Object
from bdl.entities.impl.fragment.symbol import Symbol
from bdl.entities.impl.fragment.fqn import FQN
from bdl.entities.impl.types import Category as CategoryOriginal
from bdl.visitors.composition.components import Context, ExpressionEntry
from bdl.visitors.composition.visitor import CompositionView

from rust.bdl.generator.impl.symbol import fqnToCapitalized as fqnToCapitalizedOriginal
from rust.bdl.generator.impl.symbol import symbolRustToStr


class Transform:
	Category = CategoryOriginal

	def __init__(
		self, composition: typing.Optional[CompositionView] = None, data: Optional[Dict[str, Any]] = None
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
	return template.render(bdl.tree, Transform(data=data))


def compositionRust(
	composition: CompositionView,
	data: typing.Optional[typing.Dict[str, typing.Any]] = None,
) -> str:
	template = Template.fromPath(pathlib.Path(__file__).parent / "template/composition.rs.btl", indent=True)
	return template.render(composition, Transform(composition=composition, data=data))
