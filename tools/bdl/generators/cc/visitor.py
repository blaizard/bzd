import typing
from pathlib import Path

from tools.bdl.visitors.file.visitor import Visitor
from tools.bdl.visitors.file.result import ResultType
from tools.bdl.object import Object
from tools.bdl.entities.all import Namespace, Using
from tools.bdl.entities.impl.fragment.type import Type, Visitor as VisitorType
from bzd.template.template import Template

from tools.bdl.generators.cc.types import typeToStr


def _toCamelCase(string: str) -> str:
	assert len(string), "String cannot be empty."
	return string[0].upper() + string[1:]


def _namespaceToStr(entity: Namespace) -> str:
	return "::".join(entity.nameList)


def _normalComment(comment: typing.Optional[str]) -> str:
	if comment is None:
		return ""
	if len(comment.split("\n")) > 1:
		return "/*\n{comment}\n */\n".format(comment="\n".join([" * {}".format(line) for line in comment.split("\n")]))
	return "// {comment}\n".format(comment=comment)


def _inheritanceToStr(inheritanceList: typing.List[Type]) -> str:
	return ", ".join(["public {}".format(str(typeToStr(inheritance))) for inheritance in inheritanceList])


def _bdlPathToHeader(path: Path) -> str:
	return path.as_posix().replace(".bdl", ".h")


def formatCc(bdl: Object) -> str:

	# Process the result
	result = Visitor(bdl=bdl).process()

	template = Template.fromPath(Path(__file__).parent / "template/file.h.btl", indent=True)

	result.update({
		"camelCase": _toCamelCase,
		"typeToStr": typeToStr,
		"namespaceToStr": _namespaceToStr,
		"normalComment": _normalComment,
		"inheritanceToStr": _inheritanceToStr,
		"bdlPathToHeader": _bdlPathToHeader
	})

	output = template.render(result)  # type: ignore

	return output
