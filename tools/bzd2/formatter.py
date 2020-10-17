import typing

from bzd.parser.visitor import Visitor
from bzd.parser.error import handleFromElement, assertHasAttr, assertHasSequence
from bzd.parser.element import Sequence, Element


class _FormatterTemplate(Visitor):

	nestedKind = "template"

	def visitBegin(self, result: typing.Any) -> typing.List[str]:
		return []

	def visitEnd(self, result: typing.List[str]) -> str:
		return "<{}>".format(", ".join(result))

	def visitElement(self, element: Element, result: typing.List[str]) -> typing.List[str]:
		assertHasAttr(element=element, attr="type")
		result.append(element.getAttr("type").value)
		return result

	def visitNested(self, element: Element, nestedSequence: Sequence, result: typing.List[str]) -> typing.List[str]:
		nestedResult = self.visit(nestedSequence, [])
		result[-1] += nestedResult
		return result


class Formatter(Visitor):

	nestedKind = None

	def __init__(self) -> None:
		self.level = 0

	def formatContract(self, sequence: Sequence) -> str:
		"""
		Format a contract
		"""

		contracts = []
		for element in sequence.getList():
			contract = []
			contract.append(element.getAttr("type").value)
			if element.isAttr("value"):
				contract.append("=")
				contract.append(element.getAttr("value").value)
			contracts.append(" ".join(contract))
		return "[{}]".format(", ".join(contracts))

	def formatVariableType(self, element: Element) -> str:
		"""
		Format a variable type
		"""

		assertHasAttr(element=element, attr="type")

		template = ""
		if element.isNestedSequence("template"):
			formatter = _FormatterTemplate()
			sequence = element.getNestedSequence("template")
			assert sequence is not None
			template = formatter.visit(sequence)

		return "{}{}".format(element.getAttr("type").value, template)

	def visitBegin(self, result: typing.Any) -> str:
		return ""

	def visitElement(self, element: Element, result: str) -> str:

		indent = "\t" * self.level
		assertHasAttr(element=element, attr="category")

		# Handle comments
		comment = element.getAttr("comment", "").value
		if comment:
			if len(comment.split("\n")) > 1:
				result += "{indent}/*\n{comment}\n{indent} */\n".format(indent=indent,
					comment="\n".join([" * {}{}".format(indent, line) for line in comment.split("\n")]))
			else:
				result += "{indent}// {comment}\n".format(indent=indent, comment=comment)

		# Handle class types
		if element.getAttr("category").value == "class":

			assertHasAttr(element=element, attr="name")
			assertHasSequence(element=element, sequence="nested")

			contentList = []
			contentList.append(self.formatVariableType(element))
			contentList.append(element.getAttr("name").value)
			result += "{indent}{content}\n{indent}{{\n".format(indent=indent, content=" ".join(contentList))
			self.level += 1

			sequence = element.getNestedSequence("nested")
			assert sequence is not None
			result += self.visit(sequence)

			self.level -= 1
			result += "{indent}}}".format(indent=indent)

		# Handle variable
		elif element.getAttr("category").value == "variable":

			assertHasAttr(element=element, attr="name")

			contentList = []
			if element.isAttr("const"):
				contentList.append("const")

			contentList.append(self.formatVariableType(element))
			contentList.append(element.getAttr("name").value)

			if element.isAttr("value"):
				contentList.append("=")
				contentList.append(element.getAttr("value").value)

			if element.isNestedSequence("contract"):
				sequence = element.getNestedSequence("contract")
				assert sequence is not None
				contentList.append(self.formatContract(sequence))

			result += "{indent}{content};\n".format(indent=indent, content=" ".join(contentList))

		# Should never go here
		else:
			raise Exception("Unexpected element category: {}", element.getAttr("category").value)

		return result
