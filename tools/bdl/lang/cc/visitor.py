import typing
from pathlib import Path

from tools.bdl.visitor import Visitor, VisitorType, VisitorNamespace
from tools.bdl.contracts import Contracts
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


ResultType = typing.Dict[str, typing.Any]


class _VisitorNamespace(VisitorNamespace):

	def visitNamespaceItems(self, items: typing.List[str]) -> str:
		return "::".join(items)


class CcFormatter(Visitor[ResultType]):

	def visitBegin(self, result: typing.Any) -> ResultType:
		return {"variables": {}, "classes": {}, "methods": {}, "imports": {}, "using": {}}

	def toCamelCase(self, string: str) -> str:
		assert len(string), "String cannot be empty."
		return string[0].upper() + string[1:]

	def getVariable(self, element: Element) -> typing.Any:
		contracts = []
		if element.isNestedSequence("contract"):
			sequence = element.getNestedSequence("contract")
			assert sequence
			contracts = Contracts(sequence)

		name = element.getAttrValue("name")
		assert name
		return {
			"name": name,
			"const": element.isAttr("const"),
			"type": _VisitorType(element=element).result,
			"isValue": element.isAttr("value"),
			"value": element.getAttrValue("value"),
			"comment": self.visitComment(comment=element.getAttrValue("comment")),
			"contracts": contracts
		}

	def visitComment(self, comment: typing.Optional[str]) -> str:

		if comment is None:
			return ""

		if len(comment.split("\n")) > 1:
			return "/*\n{comment}\n */\n".format(
				comment="\n".join([" * {}".format(line) for line in comment.split("\n")]))
		return "// {comment}\n".format(comment=comment)

	def visitMethod(self, result: ResultType, element: Element) -> ResultType:

		# Handle the name + arguments
		args = []
		if element.isNestedSequence("argument"):
			sequence = element.getNestedSequence("argument")
			assert sequence is not None
			for arg in sequence.iterate():
				args.append(self.getVariable(element=arg))

		name = element.getAttrValue("name")
		assert name
		result["methods"][name] = {
			"type": _VisitorType(element=element).result if element.isAttr("type") else "void",
			"comment": self.visitComment(comment=element.getAttrValue("comment")),
			"args": args
		}

		return result

	def visitVariable(self, result: ResultType, element: Element) -> ResultType:

		variable = self.getVariable(element=element)
		name = variable["name"]
		variable["nameCamelCase"] = self.toCamelCase(name)
		result["variables"][name] = variable

		return result

	def visitClass(self, result: ResultType, nestedResult: ResultType, element: Element) -> ResultType:

		name = element.getAttrValue("name")
		assert name
		result["classes"][name] = {
			"nameCamelCase": self.toCamelCase(name),
			"type": _VisitorType(element=element).result,
			"comment": self.visitComment(comment=element.getAttrValue("comment")),
			"nested": nestedResult
		}

		return result

	def visitUsing(self, result: ResultType, element: Element) -> ResultType:

		name = element.getAttrValue("name")
		assert name
		result["using"][name] = {
			"name": name,
			"type": _VisitorType(element=element).result,
			"comment": self.visitComment(comment=element.getAttrValue("comment"))
		}

		return result

	def visitNamespace(self, result: ResultType, element: Element) -> ResultType:

		result["namespace"] = _VisitorNamespace(element=element).result
		return result

	def visitFinal(self, result: ResultType) -> str:

		content = (Path(__file__).parent / "template/file.h.template").read_text()
		template = Template(content)
		output = template.process(result)

		print(output)
		print(result)

		return output

	def visitImport(self, result: ResultType, path: Path) -> ResultType:

		result["imports"][path.as_posix()] = True

		return result
