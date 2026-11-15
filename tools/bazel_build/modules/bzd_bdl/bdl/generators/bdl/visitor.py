import json
from pathlib import Path

from bzd.template.template import Template

from bdl.entities.all import Namespace
from bdl.entities.impl.fragment.parameters_resolved import ParametersResolved
from bdl.entities.impl.fragment.symbol import Symbol
from bdl.entities.impl.fragment.symbol import Visitor as VisitorSymbol
from bdl.entities.impl.types import Category
from bdl.object import Object


class _VisitorSymbol(VisitorSymbol):
	def visitSymbol(self, symbol: Symbol, nested: list[str], parameters: ParametersResolved) -> str:
		output = symbol.kind
		if symbol.const:
			output = f"const {output}"
		# Top level (aka non-template) comments are handled at a higher level.
		if not self.isTopLevel and symbol.comment:
			output = f"/*{symbol.comment}*/ {output}"
		if nested:
			output = "{}<{}>".format(output, ", ".join(nested))
		return output


def _namespaceToStr(entity: Namespace) -> str:
	return ".".join(entity.nameList)


def _symbolToStr(symbol: Symbol | None) -> str | None:
	if symbol is None:
		return None
	return _VisitorSymbol(symbol=symbol).result


def _inlineComment(comment: str | None) -> str:
	if comment is None:
		return ""
	return f"/*{comment}*/"


def _normalComment(comment: str | None) -> str:
	if comment is None:
		return ""
	return "".join([f"// {line}\n" for line in comment.split("\n")])


def _inheritanceToStr(inheritanceList: list[Symbol]) -> str:
	return ", ".join([str(_symbolToStr(inheritance)) for inheritance in inheritanceList])


def formatBdl(bdl: Object, data: Path | None = None) -> str:
	template = Template.fromPath(Path(__file__).parent / "template/file.bdl.btl", indent=True)
	output = template.render(
		bdl.tree,
		{
			"Category": Category,
			"symbolToStr": _symbolToStr,
			"namespaceToStr": _namespaceToStr,
			"inlineComment": _inlineComment,
			"normalComment": _normalComment,
			"inheritanceToStr": _inheritanceToStr,
			"data": json.loads(data.read_text()) if data else {},
		},
	)

	return output
