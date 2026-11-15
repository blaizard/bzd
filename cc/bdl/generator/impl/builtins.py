import typing

from bdl.entities.impl.fragment.symbol import Symbol
from bzd.parser.error import Error

TypeConversionCallableReturn = tuple[str, list[str]]


class IntegerType:
	constexpr: bool = True

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
			return f"bzd::Int{bits}", nested
		return f"bzd::UInt{bits}", nested


class FloatType:
	constexpr = True
	toType = "bzd::Float32"


class VoidType:
	constexpr = False
	toType = "void"


class NoneType:
	constexpr = True
	toType = "bzd::None"


class Byte:
	constexpr = True
	toType = "bzd::Byte"


class Boolean:
	constexpr = True
	toType = "bool"


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
			nested.append("void")
		if len(nested) == 1:
			nested.append("bzd::Error")
		return "bzd::Result", nested


class AsyncType:
	constexpr = False

	@staticmethod
	def toType(
		entity: Symbol,
		nested: list[str],
		reference: bool,
		values: typing.Sequence[str] | None,
	) -> TypeConversionCallableReturn:
		if len(nested) == 0:
			nested.append("void")
		if len(nested) == 1:
			nested.append("bzd::Error")
		return "bzd::Async", nested


class CallableType:
	constexpr = False
	toType = "bzd::FunctionRef<void(void)>"


class SpanType:
	constexpr = False
	toType = "bzd::Span"


class StringType:
	constexpr = True
	toType = "bzd::StringView"


class AnyType:
	constexpr = True
	toType = ""


class ListType:
	constexpr = True
	toType = ""


class ArrayType:
	name = "Array"
	constexpr = False

	@classmethod
	def toType(
		cls,
		entity: Symbol,
		nested: list[str],
		reference: bool,
		values: list[str] | None,
	) -> TypeConversionCallableReturn:
		if reference:
			return f"bzd::interface::{cls.name}", nested
		maybeContractCapacity = entity.contracts.get("capacity")
		if maybeContractCapacity is None:
			if values is None:
				# Default capacity is always 1
				nested.append("1u")
			else:
				nested.append(f"{len(values)}u")
		else:
			nested.append(f"{int(maybeContractCapacity.valueNumber):d}u")
		if values is not None:
			values.insert(0, "bzd::inPlace")
		return f"bzd::{cls.name}", nested


class VectorType(ArrayType):
	name = "Vector"


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
	"Callable": CallableType,
}
