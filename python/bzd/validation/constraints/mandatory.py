import typing

from bzd.validation.schema import Args, Constraint, ProcessedSchema


class Mandatory(Constraint):

	def install(self, processedSchema: ProcessedSchema, args: Args) -> None:
		processedSchema.setMandatory()
