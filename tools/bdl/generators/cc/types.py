import typing

from bzd.parser.error import Error

from tools.bdl.entities.impl.fragment.type import Type, Visitor
from tools.bdl.visitors.symbol_map import SymbolMap
from tools.bdl.generators.cc.comments import commentEmbeddedToStr

TypeConversionCallableReturn = typing.Tuple[str, typing.List[str]]
TypeConversionCallable = typing.Callable[[Type, typing.List[str], bool], TypeConversionCallableReturn]


def toIntegerType(entity: Type, nested: typing.List[str], reference: bool) -> TypeConversionCallableReturn:
	"""
	Convert an integer type into its C++ counterpart.
	"""

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
		else:
			Error.handleFromElement(element=entity.element, message="Value too large, max supported is 64-bit.")
	if isSigned:
		return "bzd::Int{}Type".format(bits), nested
	return "bzd::UInt{}Type".format(bits), nested


def toListType(entity: Type, nested: typing.List[str], reference: bool) -> TypeConversionCallableReturn:
	"""
	Convert a list type into its C++ counterpart.
	"""

	if reference:
		return "bzd::interface::Vector", nested
	maybeContractCapacity = entity.contracts.get("capacity")
	if maybeContractCapacity is None:
		# Default capacity is always 1
		nested.append("1U")
	else:
		nested.append("{:d}U".format(int(maybeContractCapacity.valueNumber)))
	return "bzd::Vector", nested


typeTransform: typing.Dict[str, typing.Union[str, TypeConversionCallable]] = {
	"Integer": toIntegerType,
	"Float": "float",
	"Void": "void",
	"List": toListType,
	"Result": "bzd::Result",
	"Callable": "bzd::FunctionView<void(void)>"
}


class _VisitorType(Visitor):
	"""
	Visitor to print a type.
	"""

	def __init__(self, entity: Type, updateNamespace: typing.Optional[typing.Callable[[typing.List[str]],
		typing.List[str]]], reference: bool) -> None:
		self.updateNamespace = updateNamespace
		self.reference = reference
		super().__init__(entity)

	@property
	def isReference(self) -> bool:
		return self.isTopLevel and self.reference

	@property
	def isUpdateNamespace(self) -> bool:
		return self.isTopLevel and self.updateNamespace is not None

	def visitValue(self, value: str, comment: typing.Optional[str]) -> str:

		if comment is not None:
			value = commentEmbeddedToStr(comment=comment) + " " + value

		return value

	def visitType(self, entity: Type, nested: typing.List[str]) -> str:
		"""
		Called when an element needs to be formatted.
		"""

		fqn = entity.kind
		if fqn in typeTransform:
			if callable(typeTransform[fqn]):
				output, nested = typeTransform[fqn](entity, nested, self.isReference)  # type: ignore
			else:
				output = typeTransform[fqn]
		else:
			namespace = SymbolMap.FQNToNamespace(fqn)
			if self.isUpdateNamespace:
				assert self.updateNamespace is not None
				namespace = self.updateNamespace(namespace)
			output = "::".join(namespace)

		# Apply the nested template if any
		if nested:
			output += "<{}>".format(", ".join(nested))

		if not self.isTopLevel:
			# TODO: support if there is no value
			if entity.parametersResolved:
				output += "{{{}}}".format(", ".join([expression.value for expression in entity.parametersResolved]))

		# Apply the reference if any
		if self.isReference:
			output += "&"

		# Add the comment
		if self.isTopLevel and entity.comment:
			output = commentEmbeddedToStr(comment=entity.comment) + " " + output

		return output


def typeToStr(entity: typing.Optional[Type], adapter: bool = False, reference: bool = False) -> str:
	"""
	Convert a type object into a C++ string.
	Args:
		entity: The Type entity to be converted.
		adapter: If the type should be converted into its adapter.
		reference: If the entity is passed as reference.
	"""

	if entity is None:
		return "void"
	updateNamespace = (lambda x: x[0:-1] + ["adapter"] + x[-1:]) if adapter else None
	return _VisitorType(entity=entity, updateNamespace=updateNamespace, reference=reference).result
