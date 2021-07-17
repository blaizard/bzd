import typing
from pathlib import Path

from tools.bdl.visitors.file.visitor import Visitor
from tools.bdl.visitors.file.result import ResultType
from tools.bdl.object import Object
from tools.bdl.entities.all import Namespace, Using
from tools.bdl.entities.impl.fragment.type import Type, Visitor as VisitorType
from bzd.template.template import Template


class _VisitorType(VisitorType):

	def visitTemplateItems(self, items: typing.List[str]) -> str:
		return "<{}>".format(", ".join(items))

	def visitTypeTemplate(self, kind: str, template: typing.Optional[str]) -> str:
		if template is None:
			return kind
		return "{}{}".format(kind, template)

	def visitType(self, kind: str, comment: typing.Optional[str]) -> str:

		if "." in kind:
			kind = "::".join(kind.split("."))

		if comment is None:
			return kind
		return "/*{comment}*/ {kind}".format(comment=comment, kind=kind)


def _chooseIntegerType(entity: Using) -> str:
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


def _toCamelCase(string: str) -> str:
	assert len(string), "String cannot be empty."
	return string[0].upper() + string[1:]


def _namespaceToStr(entity: Namespace) -> str:
	return "::".join(entity.nameList)


def _typeToStr(entity: typing.Optional[Type]) -> str:
	if entity is None:
		return "void"
	return _VisitorType(entity=entity).result


def _normalComment(comment: typing.Optional[str]) -> str:
	if comment is None:
		return ""
	if len(comment.split("\n")) > 1:
		return "/*\n{comment}\n */\n".format(comment="\n".join([" * {}".format(line) for line in comment.split("\n")]))
	return "// {comment}\n".format(comment=comment)


def _inheritanceToStr(inheritanceList: typing.List[Type]) -> str:
	return ", ".join(["public {}".format(str(_typeToStr(inheritance))) for inheritance in inheritanceList])


def _bdlPathToHeader(path: Path) -> str:
	return path.as_posix().replace(".bdl", ".h")


def formatCc(bdl: Object) -> str:

	# Process the result
	visitor = Visitor(bdl=bdl, resolve=True, include=True)
	result = visitor.process()

	template = Template.fromPath(Path(__file__).parent / "template/file.h.btl", indent=True)

	result.update({
		"camelCase": _toCamelCase,
		"typeToStr": _typeToStr,
		"namespaceToStr": _namespaceToStr,
		"normalComment": _normalComment,
		"inheritanceToStr": _inheritanceToStr,
		"bdlPathToHeader": _bdlPathToHeader
	})

	output = template.render(result)  # type: ignore
	return output