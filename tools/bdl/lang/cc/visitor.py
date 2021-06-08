import typing
from pathlib import Path

from tools.bdl.result import ResultType
from tools.bdl.visitor import Visitor
from tools.bdl.entity.namespace import Namespace
from tools.bdl.entity.using import Using
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
		print(kind)
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

	def chooseIntegerType(self, entity: Using) -> str:
		maybeContractMin = entity.contracts.get("min")
		isSigned = True if maybeContractMin is None or maybeContractMin.valueNumber < 0 else False
		maybeContractMax = entity.contracts.get("max")
		bits = 32
		if maybeContractMax is not None:
			maxValue = maybeContractMax.valueNumber
			if maxValue < 2**8:
				bits = 8
			elif maxValue < 2**16:
				bits = 16
			elif maxValue < 2**32:
				bits = 32
			elif maxValue < 2**64:
				bits = 64
		if isSigned:
			return "bzd::Int{}Type".format(bits)
		return "bzd::UInt{}Type".format(bits)

	def visitUsing_(self, entity: Using) -> Using:
		# Create strong types
		if entity.type.name in ["Integer", "Float"]:
			underlyingType = self.chooseIntegerType(entity)
			newType = "bzd::NamedType<{underlying}, struct {name}>".format(underlying=underlyingType, name=entity.name)
			#Type.makeCustom("bzd::NamedType", [underlyingType, "struct {name}".format(name=entity.name)])
			print("String type!", entity.name, newType)
		return entity

	def visitFinal(self, result: ResultType) -> str:

		content = (Path(__file__).parent / "template/file.h.template").read_text()
		template = Template(content)
		result.update({
			"camelCase": CcFormatter.toCamelCase,
			"typeToStr": CcFormatter.typeToStr,
			"namespaceToStr": CcFormatter.namespaceToStr,
			"normalComment": CcFormatter.normalComment
		})
		output = template.process(result, removeEmptyLines=True)

		return output
