import typing
from pathlib import Path

from bzd.template.template import Template

from tools.bdl.visitors.composition.visitor import Composition
from tools.bdl.object import Object
from tools.bdl.entities.all import Namespace, Using, Expression
from tools.bdl.entities.impl.fragment.type import Type

from tools.bdl.generators.cc.types import typeToStr as typeToStrOriginal
from tools.bdl.generators.cc.comments import commentBlockToStr as commentBlockToStrOriginal, commentEmbeddedToStr as commentEmbeddedToStrOriginal, commentParametersResolvedToStr as commentParametersResolvedToStrOriginal
from tools.bdl.generators.cc.fqn import fqnToStr as fqnToStrOriginal, fqnToAdapterStr as fqnToAdapterStrOriginal, fqnToNameStr as fqnToNameStrOriginal


# String related
class Transform:

	def __init__(self, composition: typing.Optional[Composition] = None) -> None:
		self.composition = composition

	def toCamelCase(self, string: str) -> str:
		assert len(string), "String cannot be empty."
		return string[0].upper() + string[1:]

	# Inheritance related

	def inheritanceToStr(self, inheritanceList: typing.List[Type]) -> str:
		return ", ".join(["public {}".format(str(typeToStrOriginal(inheritance))) for inheritance in inheritanceList])

	def inheritanceAdapterToStr(self, inheritanceList: typing.List[Type]) -> str:
		return ", ".join([
			"public {}<Impl>".format(str(typeToStrOriginal(inheritance, adapter=True)))
			for inheritance in inheritanceList
		])

	# Path related

	def bdlPathToHeader(self, path: Path) -> str:
		return path.as_posix().replace(".bdl", ".hh")

	# Namespace related

	def namespaceToStr(self, entity: Namespace) -> str:
		return "::".join(entity.nameList)

	# Type related

	def typeToStr(self, entity: Type) -> str:
		return typeToStrOriginal(entity=entity)

	def typeReferenceToStr(self, entity: Type) -> str:
		return typeToStrOriginal(entity=entity, reference=True)

	def typeDefinitionToStr(self, entity: Type) -> str:
		return typeToStrOriginal(entity=entity, definition=True)

	def typeNonConstToStr(self, entity: Type) -> str:
		return typeToStrOriginal(entity=entity, nonConst=True)

	def typeRegistryToStr(self, entity: Type) -> str:
		return typeToStrOriginal(entity=entity,
			registry=[entity.fqn for entity in self.composition.registry] if self.composition else None)

	# Comments

	def commentBlockToStr(self, comment: str) -> str:
		return commentBlockToStrOriginal(comment=comment)

	def commentEmbeddedToStr(self, comment: str) -> str:
		return commentEmbeddedToStrOriginal(comment=comment)

	def commentParametersResolvedToStr(self, expression: Expression) -> str:
		return commentParametersResolvedToStrOriginal(expression=expression)

	# FQNs

	def fqnToStr(self, fqn: str) -> str:
		return fqnToStrOriginal(fqn=fqn)

	def fqnToAdapterStr(self, fqn: str) -> str:
		return fqnToAdapterStrOriginal(fqn=fqn)

	def fqnToNameStr(self, fqn: str) -> str:
		return fqnToNameStrOriginal(fqn=fqn)


def formatCc(bdl: Object) -> str:

	template = Template.fromPath(Path(__file__).parent / "template/file.h.btl", indent=True)
	output = template.render(bdl.tree, Transform())

	return output


def compositionCc(composition: Composition) -> str:

	template = Template.fromPath(Path(__file__).parent / "template/composition.cc.btl", indent=True)
	output = template.render(composition, Transform(composition))

	return output
