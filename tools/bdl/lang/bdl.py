import typing
from pathlib import Path

from tools.bdl.visitor import Visitor, VisitorType, VisitorContract, VisitorNamespace
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


class _VisitorContract(VisitorContract[str, str]):

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


class _VisitorNamespace(VisitorNamespace):

	def visitNamespaceItems(self, items: typing.List[str]) -> str:
		return ".".join(items)


class BdlFormatter(Visitor[str]):

	def visitBegin(self, result: typing.Any) -> str:
		return ""

	def visitComment(self, comment: str) -> str:

		if len(comment.split("\n")) > 1:
			return "/*\n{comment}\n */\n".format(
				comment="\n".join([" * {}".format(line) for line in comment.split("\n")]))
		return "// {comment}\n".format(comment=comment)

	def _visitContractIfAny(self, contentList: typing.List[str], element: Element) -> None:

		if element.isNestedSequence("contract"):
			sequence = element.getNestedSequence("contract")
			assert sequence is not None
			visitorContract = _VisitorContract()
			contentList.append(visitorContract.visit(sequence))

	def _visitVariable(self, element: Element) -> str:

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
		self._visitContractIfAny(contentList, element)

		return " ".join(contentList)

	def visitVariable(self, result: str, element: Element) -> str:

		# Handle comments
		if element.isAttr("comment"):
			result += self.applyIndent(self.visitComment(comment=element.getAttr("comment").value))

		# Assemble
		result += self.applyIndent("{content};\n\n".format(content=self._visitVariable(element)))
		return result

	def visitMethod(self, result: str, element: Element) -> str:

		contentList = ["method"]

		# Handle the name + arguments
		nameArgs = element.getAttr("name").value + "("
		if element.isNestedSequence("argument"):
			sequence = element.getNestedSequence("argument")
			assert sequence is not None
			args = []
			for arg in sequence.iterate():
				args.append(self._visitVariable(arg))
			nameArgs += ", ".join(args)
		contentList.append(nameArgs + ")")

		# Return type
		if element.isAttr("type"):
			contentList.append("->")
			visitorType = _VisitorType(element=element)
			contentList.append(visitorType.result)

		# Handle the contract
		self._visitContractIfAny(contentList, element)

		# Handle comments
		if element.isAttr("comment"):
			result += self.applyIndent(self.visitComment(comment=element.getAttr("comment").value))

		# Assemble
		result += self.applyIndent("{content};\n\n".format(content=" ".join(contentList)))
		return result

	def visitClass(self, result: str, nestedResult: str, element: Element) -> str:

		contentList = []

		# Handle the type
		visitorType = _VisitorType(element=element)
		contentList.append(visitorType.result)

		# Handle the name
		contentList.append(element.getAttr("name").value)

		# Handle comments
		if element.isAttr("comment"):
			result += self.applyIndent(self.visitComment(comment=element.getAttr("comment").value))

		# Assemble
		result += self.applyIndent("{content}\n{{\n".format(content=" ".join(contentList)))

		result += nestedResult

		result += self.applyIndent("}\n\n")

		return result

	def visitUsing(self, result: str, element: Element) -> str:

		contentList = ["using"]

		# Handle the name
		contentList.append(element.getAttr("name").value)
		contentList.append("=")

		# Handle the type
		visitorType = _VisitorType(element=element)
		contentList.append(visitorType.result)

		# Handle the contract
		self._visitContractIfAny(contentList, element)

		# Handle comments
		if element.isAttr("comment"):
			result += self.applyIndent(self.visitComment(comment=element.getAttr("comment").value))

		result += self.applyIndent("{content};\n\n".format(content=" ".join(contentList)))

		return result

	def visitNamespace(self, result: str, element: Element) -> str:

		result += self.applyIndent("namespace {namespaces};\n\n".format(namespaces=_VisitorNamespace(element).result))

		return result

	def visitImport(self, result: str, path: Path) -> str:

		result += "import \"{path}\"\n\n".format(path=path.as_posix())

		return result
