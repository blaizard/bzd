import typing
from pathlib import Path

from tools.bdl.object import Object
from tools.bdl.entities.all import Namespace
from tools.bdl.entities.impl.fragment.type import Type, Visitor as VisitorType
from bzd.template.template import Template


class _VisitorType(VisitorType):

	def visitTemplateItems(self, items: typing.List[str]) -> str:
		return "<{}>".format(", ".join(items))

	def visitTypeTemplate(self, kind: str, template: typing.Optional[str]) -> str:
		if template is None:
			return kind
		return "{}{}".format(kind, template)

	def visitType(self, entity: Type, template: bool) -> str:
		if entity.comment is None or not template:
			return entity.kind
		return "/*{comment}*/ {kind}".format(comment=entity.comment, kind=entity.kind)


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


def formatBdl(bdl: Object) -> str:

	template = Template.fromPath(Path(__file__).parent / "template/file.bdl.btl", indent=True)
	bdl.tree.update({
		"typeToStr": _typeToStr,
		"namespaceToStr": _namespaceToStr,
		"inlineComment": _inlineComment,
		"normalComment": _normalComment,
		"inheritanceToStr": _inheritanceToStr
	})
	output = template.render(bdl.tree)  # type: ignore

	return output
