import typing

from tools.bzd2.visitor import Visitor, VisitorType, VisitorContract
from bzd.parser.element import Element


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


class Formatter(Visitor):

	def visitComment(self, comment: str) -> str:

		if len(comment.split("\n")) > 1:
			return "/*\n{comment}\n */\n".format(
				comment="\n".join([" * {}".format(line) for line in comment.split("\n")]))
		return "// {comment}\n".format(comment=comment)

	def visitVariable(self, element: Element) -> str:

		contentList = []

		# Handle the const
		if element.isAttr("const"):
			contentList.append("const")

		# Handle the type
		visitorType = _VisitorType(element=element)
		contentList.append(visitorType.result)

		# Handle the name
		contentList.append(element.getAttr("name").value)

		# Handle the value
		if element.isAttr("value"):
			contentList.append("=")
			contentList.append(element.getAttr("value").value)

		# Handle the contract
		if element.isNestedSequence("contract"):
			sequence = element.getNestedSequence("contract")
			assert sequence is not None
			visitorContract = _VisitorContract()
			contentList.append(visitorContract.visit(sequence))

		# Assemble
		return "{content};\n".format(content=" ".join(contentList))

	def visitClassBegin(self, element: Element) -> str:

		contentList = []

		# Handle the type
		visitorType = _VisitorType(element=element)
		contentList.append(visitorType.result)

		# Handle the name
		contentList.append(element.getAttr("name").value)

		# Assemble
		return "{content}\n{{\n".format(content=" ".join(contentList))

	def visitClassEnd(self, element: Element) -> str:

		return "}"
