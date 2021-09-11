import typing

from bzd.parser.error import Error

from tools.bdl.entities.all import Expression
from tools.bdl.entities.impl.fragment.type import Type, Visitor
from tools.bdl.entities.impl.fragment.parameters import ResolvedParameters
from tools.bdl.entities.impl.fragment.fqn import FQN
from tools.bdl.generators.cc.comments import commentEmbeddedToStr, commentParametersResolvedToStr
from tools.bdl.generators.cc.fqn import fqnToNameStr

TypeConversionCallableReturn = typing.Tuple[str, typing.List[str]]


class IntegerType:

	constexpr: bool = True

	@staticmethod
	def transform(entity: Type, nested: typing.List[str], reference: bool) -> TypeConversionCallableReturn:
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


class FloatType:

	constexpr = True
	transform = "float"


class VoidType:

	constexpr = False
	transform = "void"


class ByteType:

	constexpr = True
	transform = "bzd::ByteType"


class ResultType:

	constexpr = False
	transform = "bzd::Result"


class CallableType:

	constexpr = False
	transform = "bzd::FunctionView<void(void)>"


class SpanType:

	constexpr = False
	transform = "bzd::Span"


class VectorType:

	constexpr = False

	@staticmethod
	def transform(entity: Type, nested: typing.List[str], reference: bool) -> TypeConversionCallableReturn:

		if reference:
			return "bzd::interface::Vector", nested
		maybeContractCapacity = entity.contracts.get("capacity")
		if maybeContractCapacity is None:
			# Default capacity is always 1
			nested.append("1U")
		else:
			nested.append("{:d}U".format(int(maybeContractCapacity.valueNumber)))
		return "bzd::Vector", nested


knownTypes: typing.Dict[str, typing.Any] = {
	"Integer": IntegerType,
	"Float": FloatType,
	"Void": VoidType,
	"Byte": ByteType,
	"Span": SpanType,
	"Vector": VectorType,
	"Result": ResultType,
	"Callable": CallableType
}


class _VisitorType(Visitor):
	"""
	Visitor to print a type.
	"""

	def __init__(self, entity: Type, namespaceToFQN: typing.Optional[typing.Callable[[typing.List[str]], str]],
		reference: bool, definition: bool, nonConst: bool) -> None:
		self.namespaceToFQN = namespaceToFQN
		self.reference = reference
		self.definition = definition
		self.nonConst = nonConst
		super().__init__(entity)

	@property
	def isReference(self) -> bool:
		return self.isTopLevel and self.reference

	@property
	def isNamespaceToFQN(self) -> bool:
		return self.isTopLevel and self.namespaceToFQN is not None

	def visitValue(self, value: str, comment: typing.Optional[str]) -> str:

		if comment is not None:
			value = commentEmbeddedToStr(comment=comment) + " " + value

		return value

	@staticmethod
	def declareParametersResolvedValues(parameters: ResolvedParameters) -> typing.List[str]:
		"""
		This is a copy of the one defined in tools/bdl/generators/cc/template/declarations.h.btl
		"""
		content: typing.List[str] = []
		for expression in parameters:

			string = commentParametersResolvedToStr(expression)
			if expression.isType:
				if expression.isName:
					string += fqnToNameStr(expression.fqn)
				else:
					string += "{}{{{}}}".format(
						typeToStr(expression.type),
						", ".join(_VisitorType.declareParametersResolvedValues(expression.parametersResolved)))
			else:
				string += expression.value

			content.append(string)

		return content

	def visitType(self, entity: Type, nested: typing.List[str], parameters: ResolvedParameters) -> str:
		"""
		Called when an element needs to be formatted.
		"""

		# Add arguments template to the nested mix.
		nested += self.declareParametersResolvedValues(parameters)

		outputList: typing.List[str] = []
		output: str
		for index, fqn in enumerate(entity.kinds):
			if fqn in knownTypes:
				if callable(knownTypes[fqn].transform):
					output, nested = knownTypes[fqn].transform(entity, nested, self.isReference)
				else:
					output = knownTypes[fqn].transform
			else:
				namespace = FQN.toNamespace(fqn)
				if index == 0:
					if self.isNamespaceToFQN:
						assert self.namespaceToFQN is not None
						output = self.namespaceToFQN(namespace)
					else:
						output = "::".join(namespace)
				else:
					output = namespace[-1]
			outputList.append(output)
		output = ".".join(outputList)

		# Apply the nested template if any.
		if nested:
			output += "<{}>".format(", ".join(nested))

		if not self.isTopLevel:
			# TODO: support if there is no value.
			if entity.parametersResolved:
				output += "{{{}}}".format(", ".join([expression.value for expression in entity.parametersResolved]))
		else:
			if self.definition:
				if entity.underlyingType in knownTypes and knownTypes[entity.underlyingType].constexpr:
					output = "constexpr " + output

		# Apply the reference if any.
		if self.isReference:
			output += "&"

		# Apply const if needed.
		if entity.const and (not self.isTopLevel or not self.nonConst):
			output = "const " + output

		return output


def typeToStr(entity: typing.Optional[Type],
	adapter: bool = False,
	reference: bool = False,
	definition: bool = False,
	nonConst: bool = False,
	registry: typing.Optional[typing.Sequence[str]] = None) -> str:
	"""
	Convert a type object into a C++ string.
	Args:
		entity: The Type entity to be converted.
		adapter: If the type should be converted into its adapter.
		reference: If the entity is passed as reference.
		definition: The type is used for a variable definition.
		registry: FQNs that matches a registry entry.
	"""

	if entity is None:
		return "void"

	namespaceToFQN: typing.Optional[typing.Callable[[typing.List[str]], str]] = None
	if adapter:

		def namespaceToFQN(namespace: typing.List[str]) -> str:
			return "::".join(namespace[0:-1] + ["adapter"] + namespace[-1:])

	elif registry is not None:

		def namespaceToFQN(namespace: typing.List[str]) -> str:
			fqn = FQN.fromNamespace(namespace=namespace)
			assert registry is not None
			if fqn in registry:
				return "registry.{}_".format(fqnToNameStr(fqn))
			return "::".join(namespace)

	return _VisitorType(entity=entity,
		namespaceToFQN=namespaceToFQN,
		reference=reference,
		definition=definition,
		nonConst=nonConst).result
