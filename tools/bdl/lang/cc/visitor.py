import typing
from pathlib import Path

from tools.bdl.visitor import Visitor, ResultType
from tools.bdl.entity.namespace import Namespace
from tools.bdl.entity.type import Type, Visitor as VisitorType
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


class CcFormatter(Visitor):

	@staticmethod
	def toCamelCase(string: str) -> str:
		assert len(string), "String cannot be empty."
		return string[0].upper() + string[1:]

	@staticmethod
	def namespaceToStr(entity: Namespace) -> str:
		return "::".join(entity.nameList)

	@staticmethod
	def typeToStr(entity: typing.Optional[Type]) -> str:
		if entity is None:
			return "void"
		return _VisitorType(entity=entity).result

	@staticmethod
	def normalComment(comment: typing.Optional[str]) -> str:
		if comment is None:
			return ""
		if len(comment.split("\n")) > 1:
			return "/*\n{comment}\n */\n".format(
				comment="\n".join([" * {}".format(line) for line in comment.split("\n")]))
		return "// {comment}\n".format(comment=comment)

	def visitFinal(self, result: ResultType) -> str:

		content = (Path(__file__).parent / "template/file.h.template").read_text()
		template = Template(content)
		result["camelCase"] = CcFormatter.toCamelCase
		result["typeToStr"] = CcFormatter.typeToStr
		result["namespaceToStr"] = CcFormatter.namespaceToStr
		result["normalComment"] = CcFormatter.normalComment
		output = template.process(result)

		print(output)
		print(result)

		return output
