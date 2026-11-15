from bzd.validation.constraints.impl.number import Number
from bzd.validation.schema import TypeContext


class Float(Number[float]):
	def check(self, context: TypeContext) -> None:
		context.setUnderlying(self._toFloat(context.value))
