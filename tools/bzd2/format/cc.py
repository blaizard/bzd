import typing
from pathlib import Path

from tools.bzd2.visitor import Visitor, VisitorType, VisitorContract
from bzd.parser.element import Element
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


class _VisitorContract(VisitorContract):

	def visitContractItems(self, items: typing.List[str]) -> str:
		return "[{}]".format(", ".join(items))

	def visitContract(self, kind: str, value: typing.Optional[str], comment: typing.Optional[str]) -> str:

		items = []
		if comment is not None:
			items.append("/*{}*/".format(comment))
		items.append(kind)
		if value is not None:
			items.append("=")
			items.append(value)

		return " ".join(items)


ResultType = typing.Dict[str, typing.Any]


class CcFormatter(Visitor[ResultType]):

	def visitBegin(self, result: typing.Any) -> ResultType:
		return {"variables": {}, "classes": {}}

	def visitComment(self, comment: typing.Optional[str]) -> typing.Optional[str]:

		if comment is None:
			return None

		if len(comment.split("\n")) > 1:
			return "/*\n{comment}\n */\n".format(
				comment="\n".join([" * {}".format(line) for line in comment.split("\n")]))
		return "// {comment}\n".format(comment=comment)

	def visitVariable(self, result: ResultType, element: Element) -> ResultType:

		name = element.getAttrValue("name")
		result["variables"][name] = {
			"const": element.isAttr("const"),
			"type": _VisitorType(element=element).result,
			"isValue": element.isAttr("value"),
			"value": element.getAttrValue("value"),
			"comment": self.visitComment(comment=element.getAttrValue("comment"))
		}

		return result

	def visitClass(self, result: ResultType, nestedResult: ResultType, element: Element) -> ResultType:

		name = element.getAttrValue("name")
		result["classes"][name] = {
			"type": _VisitorType(element=element).result,
			"comment": self.visitComment(comment=element.getAttrValue("comment")),
			"nested": nestedResult
		}

		return result

	def visitFinal(self, result: ResultType) -> str:

		print(result)

		content = (Path(__file__).parent / "template/cc/class.h.template").read_text()
		template = Template(content)
		output = template.process(result)

		print(output)

		return output
