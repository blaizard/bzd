import typing

from bzd.validation.schema import Constraint, ProcessedSchema


class Mandatory(Constraint):

	def install(self, processedSchema: ProcessedSchema, args: typing.List[str]) -> None:
		self.validate(schema=[], values=args)
		processedSchema.setMandatory()
