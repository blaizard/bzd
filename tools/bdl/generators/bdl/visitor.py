import typing
from pathlib import Path

from tools.bdl.object import Object
from tools.bdl.entities.all import Namespace
from tools.bdl.entities.impl.fragment.type import Type, Visitor as VisitorType
from tools.bdl.entities.impl.fragment.parameters_resolved import ParametersResolved
from bzd.template.template import Template

class _VisitorType(VisitorType):

	def visitType(self, entity: Type, nested: typing.List[str], parameters: ParametersResolved) -> str:
		output = entity.kind
		if entity.const:
			output = "const {}".format(output)
		# Top level (aka non-template) comments are handled at a higher level.
		if not self.isTopLevel and entity.comment:
			output = "/*{comment}*/ {output}".format(comment=entity.comment, output=output)
		if nested:
			output = "{}<{}>".format(output, ", ".join(nested))
		return output


def _namespaceToStr(entity: Namespace) -> str:
	return ".".join(entity.nameList)


def _typeToStr(entity: typing.Optional[Type]) -> typing.Optional[str]:
	if entity is None:
		return None
	return _VisitorType(entity=entity).result


def _inlineComment(comment: typing.Optional[str]) -> str:
	if comment is None:
		return ""
	return "/*{}*/".format(comment)


def _normalComment(comment: typing.Optional[str]) -> str:
	if comment is None:
		return ""
	if len(comment.split("\n")) > 1:
		return "/*\n{comment}\n */\n".format(comment="\n".join([" * {}".format(line) for line in comment.split("\n")]))
	return "// {comment}\n".format(comment=comment)


def _inheritanceToStr(inheritanceList: typing.List[Type]) -> str:
	return ", ".join([str(_typeToStr(inheritance)) for inheritance in inheritanceList])


def formatBdl(bdl: Object, includes: typing.List[Path]) -> str:

	template = Template.fromPath(Path(__file__).parent / "template/file.bdl.btl", indent=True)
	output = template.render(
		bdl.tree, {
		"typeToStr": _typeToStr,
		"namespaceToStr": _namespaceToStr,
		"inlineComment": _inlineComment,
		"normalComment": _normalComment,
		"inheritanceToStr": _inheritanceToStr
		})

	return output
