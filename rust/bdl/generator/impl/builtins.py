import typing

from bdl.entities.impl.fragment.symbol import Symbol
from bzd.parser.error import Error

TypeConversionCallableReturn = tuple[str, list[str]]


class IntegerType:
	constexpr: bool = True
	isCopy: bool = True

	@staticmethod
	def toType(
		entity: Symbol,
		nested: list[str],
		reference: bool,
		values: typing.Sequence[str] | None,
	) -> TypeConversionCallableReturn:
		maybeContractMin = entity.contracts.get("min")
		isSigned = bool(maybeContractMin is None or maybeContractMin.valueNumber < 0)
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
			return f"i{bits}", nested
		return f"u{bits}", nested


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
		nested: list[str],
		reference: bool,
		values: typing.Sequence[str] | None,
	) -> TypeConversionCallableReturn:
		element = nested[0] if nested else "()"
		return f"&'static [{element}]", []


class ArrayType:
	constexpr = False

	@staticmethod
	def toType(
		entity: Symbol,
		nested: list[str],
		reference: bool,
		values: typing.Sequence[str] | None,
	) -> TypeConversionCallableReturn:
		maybeContractCapacity = entity.contracts.get("capacity")
		capacity = int(maybeContractCapacity.valueNumber) if maybeContractCapacity else 1
		element = nested[0] if nested else "()"
		return f"&[{element}; {capacity}]", []


class VectorType:
	constexpr = False

	@staticmethod
	def toType(
		entity: Symbol,
		nested: list[str],
		reference: bool,
		values: typing.Sequence[str] | None,
	) -> TypeConversionCallableReturn:
		element = nested[0] if nested else "()"
		return f"&[{element}]", []


class ResultType:
	constexpr = False

	@staticmethod
	def toType(
		entity: Symbol,
		nested: list[str],
		reference: bool,
		values: typing.Sequence[str] | None,
	) -> TypeConversionCallableReturn:
		if len(nested) == 0:
			nested.append("()")
		if len(nested) == 1:
			nested.append("bzd::base::error::Error")
		return "Result", nested


builtins: dict[str, typing.Any] = {
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
