import typing
from pathlib import Path

from tools.bdl.visitor import Visitor
from tools.bdl.entity.contract import Contracts
from tools.bdl.entity.variable import Variable
from tools.bdl.entity.nested import Nested
from tools.bdl.entity.method import Method
from tools.bdl.entity.using import Using
from tools.bdl.entity.namespace import Namespace
from tools.bdl.entity.use import Use
from tools.bdl.entity.type import Type, Visitor as VisitorType
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


class BdlFormatter(Visitor[str]):

	def visitBegin(self, result: typing.Any) -> str:
		return ""

	def updateComment(self, comment: str) -> str:

		if len(comment.split("\n")) > 1:
			return "/*\n{comment}\n */\n".format(
				comment="\n".join([" * {}".format(line) for line in comment.split("\n")]))
		return "// {comment}\n".format(comment=comment)

	def _visitContractIfAny(self, contentList: typing.List[str], element: Element) -> None:

		if element.isNestedSequence("contract"):
			sequence = element.getNestedSequence("contract")
			assert sequence is not None
			contracts = Contracts(sequence, visitor=self)

			items = []
			for contract in contracts:
				temp = []
				if contract.comment is not None:
					temp.append("/*{}*/".format(contract.comment))
				temp.append(contract.type)
				if contract.value is not None:
					temp.append("=")
					temp.append(contract.value)
				items.append(" ".join(temp))

			contentList.append("[{}]".format(", ".join(items)))

	def _visitVariable(self, element: Element) -> str:

		contentList = []

		# Handle the const
		if element.isAttr("const"):
			contentList.append("const")

		# Handle the type
		typeStr = Type(element=element).visit(_VisitorType)
		contentList.append(typeStr)

		# Handle the name
		contentList.append(element.getAttr("name").value)

		# Handle the value
		if element.isAttr("value"):
			contentList.append("=")
			contentList.append(element.getAttr("value").value)

		# Handle the contract
		self._visitContractIfAny(contentList, element)

		return " ".join(contentList)

	def visitVariable(self, result: str, element: Element, entity: Variable) -> str:

		# Handle comments
		if element.isAttr("comment"):
			result += self.applyIndent(self.updateComment(comment=element.getAttr("comment").value))

		# Assemble
		result += self.applyIndent("{content};\n\n".format(content=self._visitVariable(element)))
		return result

	def visitMethod(self, result: str, element: Element, entity: Method) -> str:

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
			typeStr = Type(element=element).visit(_VisitorType)
			contentList.append(typeStr)

		# Handle the contract
		self._visitContractIfAny(contentList, element)

		# Handle comments
		if element.isAttr("comment"):
			result += self.applyIndent(self.updateComment(comment=element.getAttr("comment").value))

		# Assemble
		result += self.applyIndent("{content};\n\n".format(content=" ".join(contentList)))
		return result

	def visitNested(self, result: str, nestedResult: str, element: Element, entity: Nested) -> str:

		contentList = []

		# Handle the type
		typeStr = Type(element=element).visit(_VisitorType)
		contentList.append(typeStr)

		# Handle the name
		contentList.append(element.getAttr("name").value)

		# Handle comments
		if element.isAttr("comment"):
			result += self.applyIndent(self.updateComment(comment=element.getAttr("comment").value))

		# Assemble
		result += self.applyIndent("{content}\n{{\n".format(content=" ".join(contentList)))

		result += nestedResult

		result += self.applyIndent("}\n\n")

		return result

	def visitUsing(self, result: str, element: Element, entity: Using) -> str:

		contentList = ["using"]

		# Handle the name
		contentList.append(element.getAttr("name").value)
		contentList.append("=")

		# Handle the type
		typeStr = Type(element=element).visit(_VisitorType)
		contentList.append(typeStr)

		# Handle the contract
		self._visitContractIfAny(contentList, element)

		# Handle comments
		if element.isAttr("comment"):
			result += self.applyIndent(self.updateComment(comment=element.getAttr("comment").value))

		result += self.applyIndent("{content};\n\n".format(content=" ".join(contentList)))

		return result

	def visitNamespace(self, result: str, element: Element, entity: Namespace) -> str:

		result += self.applyIndent("namespace {namespaces};\n\n".format(namespaces=".".join(entity.nameList)))

		return result

	def visitUse(self, result: str, entity: Use) -> str:

		result += "use \"{path}\"\n\n".format(path=entity.path.as_posix())

		return result
