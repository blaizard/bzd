from bzd.validation.constraints.impl.number import Number
from bzd.validation.schema import TypeContext


class Integer(Number[int]):
	def check(self, context: TypeContext) -> None:
		context.setUnderlying(self._toInteger(context.value))
