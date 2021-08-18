import typing

from tools.bdl.entities.impl.fragment.type import Type, Visitor
from tools.bdl.visitors.symbol_map import SymbolMap
from tools.bdl.generators.cc.comments import commentEmbeddedToStr

typeTransform = {"Integer": 12}
"""
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
"""


class _VisitorType(Visitor):
	"""
	Visitor to print a type.
	"""

	def __init__(
			self,
			entity: Type,
			updateNamespace: typing.Optional[typing.Callable[[typing.List[str]], typing.List[str]]] = None) -> None:
		self.updateNamespace = updateNamespace
		super().__init__(entity)

	def visitTemplateItems(self, items: typing.List[str]) -> str:
		return "<{}>".format(", ".join(items))

	def visitTypeTemplate(self, kind: str, template: typing.Optional[str]) -> str:
		if template is None:
			return kind
		return "{}{}".format(kind, template)

	def visitType(self, entity: Type, template: bool) -> str:

		namespace = SymbolMap.FQNToNamespace(entity.kind)
		if not template and self.updateNamespace is not None:
			namespace = self.updateNamespace(namespace)
		output = "::".join(namespace)

		if template and entity.comment is not None:
			output = commentEmbeddedToStr(comment=entity.comment) + " " + output

		return output

	def visitValue(self, value: str, comment: typing.Optional[str]) -> str:

		if comment is not None:
			value = commentEmbeddedToStr(comment=comment) + " " + value

		return value


def typeToStr(entity: typing.Optional[Type], adapter: bool = False) -> str:
	"""
	Convert a type object into a C++ string.
	Args:
		entity: The Type entity to be converted.
		adapter: If the type should be converted into its adapter.
	"""

	if entity is None:
		return "void"
	updateNamespace = (lambda x: x[0:-1] + ["adapter"] + x[-1:]) if adapter else None
	return _VisitorType(entity=entity, updateNamespace=updateNamespace).result
