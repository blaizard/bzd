import typing
from pathlib import Path

from tools.bdl.result import ResultType
from tools.bdl.entity.namespace import Namespace
from tools.bdl.entity.fragment.type import Type, Visitor as VisitorType
from bzd.template.template import Template


class _VisitorType(VisitorType):

	def visitTemplateItems(self, items: typing.List[str]) -> str:
		return "<{}>".format(", ".join(items))

	def visitTypeTemplate(self, kind: str, template: typing.Optional[str]) -> str:
		if template is None:
			return kind
		return "{}{}".format(kind, template)

	def visitType(self, kind: str, comment: typing.Optional[str]) -> str:
		if comment is None:
			return kind
		return "/*{comment}*/ {kind}".format(comment=comment, kind=kind)


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


def formatBdl(result: ResultType) -> str:

	content = (Path(__file__).parent / "template/file.bdl.template").read_text()
	template = Template(content)
	result.update({
		"typeToStr": _typeToStr,
		"namespaceToStr": _namespaceToStr,
		"inlineComment": _inlineComment,
		"normalComment": _normalComment
	})
	output = template.process(result, removeEmptyLines=True)

	return output
