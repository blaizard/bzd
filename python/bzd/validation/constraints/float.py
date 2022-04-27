import typing

from bzd.validation.schema import TypeContext
from bzd.validation.constraints.impl.number import Number


class Float(Number[float]):

	def check(self, context: TypeContext) -> None:
		context.setUnderlying(self._toFloat(context.value))
