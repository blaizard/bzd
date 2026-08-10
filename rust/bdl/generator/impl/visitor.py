import typing
import pathlib

from typing import Any, Dict, Optional

from bzd.template.template import Template
from bdl.object import Object
from bdl.entities.impl.fragment.symbol import Symbol
from bdl.entities.impl.types import Category as CategoryOriginal

from rust.bdl.generator.impl.symbol import symbolRustToStr


class Transform:
	Category = CategoryOriginal

	def __init__(self, data: Optional[Dict[str, Any]] = None) -> None:
		self.data = data if data else {}

	def symbolToStr(self, symbol: Optional[Symbol]) -> str:
		return symbolRustToStr(symbol)

	def symbolNonConstToStr(self, symbol: Optional[Symbol]) -> str:
		return symbolRustToStr(symbol, nonConst=True)

	def symbolReferenceToStr(self, symbol: Optional[Symbol]) -> str:
		return symbolRustToStr(symbol, reference=True)


def formatRust(bdl: Object, data: typing.Optional[typing.Dict[str, typing.Any]] = None) -> str:
	template = Template.fromPath(pathlib.Path(__file__).parent / "template/file.rs.btl", indent=True)
	output = template.render(bdl.tree, Transform(data=data))

	return output
