import typing

from bzd.validation.schema import Result, TypeContext
from bzd.validation.constraints.impl.number import Number


class Float(Number[float]):

	def check(self, context: TypeContext) -> Result:
		try:
			context.setUnderlying(self._toFloat(context.value))
		except Exception as e:
			return str(e)
		return None
