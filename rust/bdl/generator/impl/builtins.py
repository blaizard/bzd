import typing

from bzd.parser.error import Error
from bdl.entities.impl.fragment.symbol import Symbol

TypeConversionCallableReturn = typing.Tuple[str, typing.List[str]]


class IntegerType:
	constexpr: bool = True
	isCopy: bool = True

	@staticmethod
	def toType(
		entity: Symbol,
		nested: typing.List[str],
		reference: bool,
		values: typing.Optional[typing.Sequence[str]],
	) -> TypeConversionCallableReturn:
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
				Error.handleFromElement(
					element=entity.element,
					message="Value too large, max supported is 64-bit.",
				)
		if isSigned:
			return "i{}".format(bits), nested
		return "u{}".format(bits), nested


class FloatType:
	constexpr = True
	isCopy: bool = True
	toType = "f32"


class VoidType:
	constexpr = False
	toType = "()"


class NoneType:
	constexpr = True
	toType = "()"


class Byte:
	constexpr = True
	isCopy: bool = True
	toType = "u8"


class Boolean:
	constexpr = True
	isCopy: bool = True
	toType = "bool"


class StringType:
	constexpr = True
	toType = "&'static str"


class AnyType:
	constexpr = True
	toType = ""


class ListType:
	constexpr = True
	toType = ""


class SpanType:
	constexpr = False

	@staticmethod
	def toType(
		entity: Symbol,
		nested: typing.List[str],
		reference: bool,
		values: typing.Optional[typing.Sequence[str]],
	) -> TypeConversionCallableReturn:
		element = nested[0] if nested else "()"
		return "&'static [{}]".format(element), []


class ArrayType:
	constexpr = False

	@staticmethod
	def toType(
		entity: Symbol,
		nested: typing.List[str],
		reference: bool,
		values: typing.Optional[typing.Sequence[str]],
	) -> TypeConversionCallableReturn:
		maybeContractCapacity = entity.contracts.get("capacity")
		capacity = int(maybeContractCapacity.valueNumber) if maybeContractCapacity else 1
		element = nested[0] if nested else "()"
		return "&[{}; {}]".format(element, capacity), []


class VectorType:
	constexpr = False

	@staticmethod
	def toType(
		entity: Symbol,
		nested: typing.List[str],
		reference: bool,
		values: typing.Optional[typing.Sequence[str]],
	) -> TypeConversionCallableReturn:
		element = nested[0] if nested else "()"
		return "&[{}]".format(element), []


class ResultType:
	constexpr = False

	@staticmethod
	def toType(
		entity: Symbol,
		nested: typing.List[str],
		reference: bool,
		values: typing.Optional[typing.Sequence[str]],
	) -> TypeConversionCallableReturn:
		if len(nested) == 0:
			nested.append("()")
		if len(nested) == 1:
			nested.append("bzd::base::error::Error")
		return "Result", nested


builtins: typing.Dict[str, typing.Any] = {
	"Any": AnyType,
	"list": ListType,
	"Integer": IntegerType,
	"Float": FloatType,
	"None": NoneType,
	"Void": VoidType,
	"Byte": Byte,
	"Boolean": Boolean,
	"String": StringType,
	"Span": SpanType,
	"Array": ArrayType,
	"Vector": VectorType,
	"Result": ResultType,
}
