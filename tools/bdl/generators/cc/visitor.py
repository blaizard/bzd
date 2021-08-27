import typing
from pathlib import Path

from bzd.template.template import Template

from tools.bdl.visitors.symbol_map import SymbolMap
from tools.bdl.visitors.composition.visitor import Composition
from tools.bdl.object import Object
from tools.bdl.entities.all import Namespace, Using
from tools.bdl.entities.impl.fragment.type import Type

from tools.bdl.generators.cc.types import typeToStr
from tools.bdl.generators.cc.comments import commentBlockToStr, commentEmbeddedToStr, commentParametersResolvedToStr
from tools.bdl.generators.cc.fqn import fqnToStr, fqnToAdapterStr, fqnToNameStr

# String related


def toCamelCase(string: str) -> str:
	assert len(string), "String cannot be empty."
	return string[0].upper() + string[1:]


# Inheritance related


def inheritanceToStr(inheritanceList: typing.List[Type]) -> str:
	return ", ".join(["public {}".format(str(typeToStr(inheritance))) for inheritance in inheritanceList])


def inheritanceAdapterToStr(inheritanceList: typing.List[Type]) -> str:
	return ", ".join(
		["public {}<Impl>".format(str(typeToStr(inheritance, adapter=True))) for inheritance in inheritanceList])


# Path related


def bdlPathToHeader(path: Path) -> str:
	return path.as_posix().replace(".bdl", ".h")


# Namespace related


def namespaceToStr(entity: Namespace) -> str:
	return "::".join(entity.nameList)


# Type related


def typeReferenceToStr(entity: Type) -> str:
	return typeToStr(entity=entity, reference=True)


def typeDefinitionToStr(entity: Type) -> str:
	return typeToStr(entity=entity, definition=True)


transforms = {
	"toCamelCase": toCamelCase,
	"typeToStr": typeToStr,
	"typeReferenceToStr": typeReferenceToStr,
	"typeDefinitionToStr": typeDefinitionToStr,
	"namespaceToStr": namespaceToStr,
	"commentBlockToStr": commentBlockToStr,
	"commentEmbeddedToStr": commentEmbeddedToStr,
	"commentParametersResolvedToStr": commentParametersResolvedToStr,
	"inheritanceToStr": inheritanceToStr,
	"inheritanceAdapterToStr": inheritanceAdapterToStr,
	"bdlPathToHeader": bdlPathToHeader,
	"fqnToStr": fqnToStr,
	"fqnToAdapterStr": fqnToAdapterStr,
	"fqnToNameStr": fqnToNameStr
}


def formatCc(bdl: Object) -> str:

	template = Template.fromPath(Path(__file__).parent / "template/file.h.btl", indent=True)

	output = template.render(bdl.tree, transforms)

	return output


def compositionCc(composition: Composition) -> str:

	template = Template.fromPath(Path(__file__).parent / "template/main.cc.btl", indent=True)

	print(composition.registry)
	output = template.render(composition, transforms)

	return output
