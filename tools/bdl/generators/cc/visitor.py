import typing
from pathlib import Path

from bzd.template.template import Template

from tools.bdl.visitors.composition.visitor import Composition, AsyncType
from tools.bdl.object import Object
from tools.bdl.entities.all import Namespace, Using, Expression
from tools.bdl.entities.impl.fragment.type import Type
from tools.bdl.entities.impl.fragment.parameters_resolved import ParametersResolved, ParametersResolvedItem
from tools.bdl.entities.impl.types import Category

from tools.bdl.generators.cc.types import typeToStr as typeToStrOriginal
from tools.bdl.generators.cc.value import valueToStr as valueToStrOriginal
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

	Category = Category
	AsyncType = AsyncType

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

	# ParameterResolvedItem related

	def paramsConstructorDefinition(self, name: str, params: ParametersResolved) -> str:
		args = []
		assigns = []
		for index, item in enumerate(params):
			if item.param.isLiteral:
				pass
			elif item.isLValue:
				args.append(f"T{index}& {item.name}_")
				assigns.append(f"{item.name}{{{item.name}_}}")
			elif item.isRValue:
				args.append(f"T{index}&& {item.name}_")
				assigns.append(f"{item.name}{{bzd::move({item.name}_)}}")
		if not args:
			return f"{name}() noexcept = default;"
		return f"constexpr {name}({', '.join(args)}) noexcept : {', '.join(assigns)} {{}}"

	def paramsTemplateDefinition(self, params: ParametersResolved) -> str:
		args = []
		for index, item in enumerate(params):
			if not item.param.isLiteral:
				args.append(f"class T{index}")
		if not args:
			return ""
		return f"template <{', '.join(args)}>"

	def paramToDefinition(self, item: ParametersResolvedItem, index: int) -> str:

		if item.param.isLiteral:
			return f"static constexpr {typeToStrOriginal(item.type)} {item.name}{{{item.param.literal}}};"
		elif item.isLValue:
			return f"T{index}& {item.name};"
		elif item.isRValue:
			return f"T{index} {item.name};"
		item.error(message="Type not supported, should never happen.")

	def paramsDeclarationToList_(self, params: ParametersResolved, isRegistry: bool = False) -> typing.List[str]:
		"""Declare inline parameters.

		Args:
			params: The parameters to be serialized.
			isRegistry: Use registry variable instead of direct variables.
		"""
		registry = self.composition.registry.keys() if self.composition and isRegistry else None
		symbols = self.composition.symbols if self.composition else None
		return [valueToStrOriginal(item, symbols=symbols, registry=registry) for item in params]

	def paramsDeclaration(self, params: ParametersResolved, isRegistry: bool = False) -> str:
		return ", ".join(self.paramsDeclarationToList_(params=params, isRegistry=isRegistry))

	def paramsFilterOutLiterals(self, params: ParametersResolved) -> typing.Iterator[ParametersResolved]:
		for item in params:
			if item.param.isLiteral:
				continue
			yield item

	# Expression related

	def expressionToValue(self, entity: Expression) -> str:
		return valueToStrOriginal(item=entity.toParametersResolvedItem())

	def expressionToDefinition(self, entity: Expression) -> str:
		"""
		normal           -> int myvar{value}
		no default value -> int myvar
		unamed           -> int{value}
		"""
		output = ""
		values = self.paramsDeclarationToList_(
			params=entity.parametersResolved) if entity.parametersResolved.size() else []
		entity.assertTrue(condition=entity.isType, message="An expression declaration must have a type.")
		output += f" {typeToStrOriginal(entity.type, referenceForInterface=True, values=values)}"
		if entity.isName:
			output += f" {entity.name}"
		if values:
			output += f"{{{', '.join(values)}}}"

		return output

	# Comments

	def commentBlockToStr(self, comment: str) -> str:
		return commentBlockToStrOriginal(comment=comment)

	def commentEmbeddedToStr(self, comment: str) -> str:
		return commentEmbeddedToStrOriginal(comment=comment)

	def commentParametersResolvedToStr(self, item: ParametersResolvedItem) -> str:
		return commentParametersResolvedToStrOriginal(item=item)

	# FQNs

	def fqnToStr(self, fqn: str) -> str:
		return fqnToStrOriginal(fqn=fqn)

	def fqnToAdapterStr(self, fqn: str) -> str:
		return fqnToAdapterStrOriginal(fqn=fqn)

	def fqnToNameStr(self, fqn: str) -> str:
		return fqnToNameStrOriginal(fqn=fqn)

	# Async type

	def asyncTypeToStr(self, asyncType: AsyncType) -> str:
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
