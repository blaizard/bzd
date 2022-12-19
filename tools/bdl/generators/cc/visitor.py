import typing
from pathlib import Path

from bzd.template.template import Template

from tools.bdl.visitors.composition.visitor import Composition, AsyncType
from tools.bdl.object import Object
from tools.bdl.entities.all import Namespace, Using, Expression
from tools.bdl.entities.impl.fragment.type import Type

from tools.bdl.generators.cc.types import typeToStr as typeToStrOriginal
from tools.bdl.generators.cc.values import valueToStr as valueToStrOriginal
from tools.bdl.generators.cc.comments import commentBlockToStr as commentBlockToStrOriginal, commentEmbeddedToStr as commentEmbeddedToStrOriginal, commentParametersResolvedToStr as commentParametersResolvedToStrOriginal
from tools.bdl.generators.cc.fqn import fqnToStr as fqnToStrOriginal, fqnToAdapterStr as fqnToAdapterStrOriginal, fqnToNameStr as fqnToNameStrOriginal

"""
Use cases:

Interfaces: (abstract notion)
- component name : inteface {}  <- Should be set as a normal type.
- var = interface&              <- When used it should always be as a reference.

Expressions:
- Can be defined:
	- var = Type{1};
	- var = MyComponent{2};
- Can be used in config:
	- var = MyInterface&;
	- literals are static constexpr.
	- Anything that reference a variable is reference.
	- Builtins and struct are copy.
- Can be used in function parameters (declaration):
	- 
"""

# String related
class Transform:

	def __init__(self, composition: typing.Optional[Composition] = None, includes: typing.List[Path] = []) -> None:
		self.composition = composition
		self.includes = includes

	def toCamelCase(self, string: str) -> str:
		assert len(string), "String cannot be empty."
		return string[0].upper() + string[1:]

	# Inheritance related

	def inheritanceToStr(self, inheritanceList: typing.List[Type]) -> str:
		return ", ".join([f"public {str(typeToStrOriginal(inheritance))}<Impl>" for inheritance in inheritanceList])

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
		return typeToStrOriginal(entity=entity, reference=True, referenceForInterface=True)

	def typeDefinitionToStr(self, entity: Type) -> str:
		return typeToStrOriginal(entity=entity, definition=True, referenceForInterface=True)

	def typeNonConstToStr(self, entity: Type) -> str:
		return typeToStrOriginal(entity=entity, nonConst=True)

	def typeRegistryToStr(self, entity: Type) -> str:
		return typeToStrOriginal(entity=entity,
			registry=self.composition.registry.keys() if self.composition else None)  # type: ignore

	# Expression related

	def valueToRValue(self, entity: Expression) -> str:
		return valueToStrOriginal(entity=entity)

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

	# Async type

	def asyncTypeToStr(self, asyncType: str) -> str:
		return {
			AsyncType.workload: "bzd::async::Type::workload",
			AsyncType.service: "bzd::async::Type::service",
		}[asyncType]

	# Platform
	def isPlatform(self, expression: Expression) -> bool:
		return expression.fqn.startswith("bzd.platform")

	# Parameter
	def parametersResolvedToStr(self, expression: Expression) -> str:
		if expression.element.isAttr("key"):
			return expression.element.getAttr("key").value
		return ""


def formatCc(bdl: Object, includes: typing.List[Path]) -> str:

	template = Template.fromPath(Path(__file__).parent / "template/file.h.btl", indent=True)
	output = template.render(bdl.tree, Transform(includes=includes))

	return output


def compositionCc(composition: Composition) -> str:

	template = Template.fromPath(Path(__file__).parent / "template/composition.cc.btl", indent=True)
	output = template.render(composition, Transform(composition=composition))

	return output
