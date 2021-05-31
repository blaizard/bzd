import typing
from pathlib import Path

from tools.bdl.visitor import Visitor
from tools.bdl.entity.variable import Variable
from tools.bdl.entity.nested import Nested
from tools.bdl.entity.method import Method
from tools.bdl.entity.using import Using
from tools.bdl.entity.namespace import Namespace
from tools.bdl.entity.use import Use
from tools.bdl.entity.type import Visitor as VisitorType
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


class BdlFormatter(Visitor[ResultType]):

	def visitBegin(self, result: typing.Any) -> ResultType:
		return {"variables": {}, "classes": {}, "methods": {}, "uses": {}, "using": {}, "namespace": False}

	def visitComment(self, context: str, comment: typing.Optional[str]) -> str:

		if comment is None:
			return ""

		if context == "contract":
			return "/*{}*/".format(comment)

		if len(comment.split("\n")) > 1:
			return "/*\n{comment}\n */\n".format(
				comment="\n".join([" * {}".format(line) for line in comment.split("\n")]))
		return "// {comment}\n".format(comment=comment)

	def visitMethod(self, result: ResultType, element: Element, entity: Method) -> ResultType:

		args = []
		for arg in entity.args:
			args.append({
				"name": arg.name,
				"const": arg.const,
				"type": arg.type.visit(_VisitorType),
				"value": arg.value,
				"isValue": arg.isValue,
				"comment": arg.comment,
				"contracts": arg.contracts
			})

		name = entity.name
		result["methods"][name] = {
			"name": name,
			"isType": entity.isType,
			"type": entity.type.visit(_VisitorType) if entity.type else None,
			"comment": entity.comment,
			"args": args
		}

		return result

	def visitVariable(self, result: ResultType, element: Element, entity: Variable) -> ResultType:

		name = entity.name
		result["variables"][name] = {
			"name": name,
			"const": entity.const,
			"type": entity.type.visit(_VisitorType),
			"value": entity.value,
			"isValue": entity.isValue,
			"comment": entity.comment,
			"contracts": entity.contracts
		}

		return result

	def visitNested(self, result: ResultType, nestedResult: ResultType, element: Element, entity: Nested) -> ResultType:

		name = entity.name
		result["classes"][name] = {
			"name": name,
			"type": entity.type.visit(_VisitorType),
			"comment": entity.comment,
			"nested": entity.nested
		}

		return result

	def visitUsing(self, result: ResultType, element: Element, entity: Using) -> ResultType:

		name = entity.name
		result["using"][name] = {
			"name": name,
			"contracts": entity.contracts,
			"type": entity.type.visit(_VisitorType),
			"comment": entity.comment,
		}

		return result

	def visitNamespace(self, result: ResultType, element: Element, entity: Namespace) -> ResultType:

		result["namespace"] = ".".join(entity.nameList)
		return result

	def visitUse(self, result: ResultType, entity: Use) -> ResultType:

		result["uses"][entity.path.as_posix()] = True

		return result

	def visitFinal(self, result: ResultType) -> str:

		content = (Path(__file__).parent / "template/file.bdl.template").read_text()
		template = Template(content)
		print(result)
		output = template.process(result)

		print(output)
		print(result)

		return output
