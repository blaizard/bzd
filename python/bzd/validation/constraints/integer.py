import typing

from bzd.validation.schema import TypeContext
from bzd.validation.constraints.impl.number import Number


class Integer(Number[int]):

	def check(self, context: TypeContext) -> None:
		context.setUnderlying(self._toInteger(context.value))
