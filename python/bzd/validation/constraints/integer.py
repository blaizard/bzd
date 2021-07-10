import typing

from bzd.validation.schema import Constraint, ValidationCallable, ProcessedSchema


class Integer(Constraint):

	def install(self, processedSchema: ProcessedSchema) -> typing.Optional[ValidationCallable]:
		return lambda value: True
