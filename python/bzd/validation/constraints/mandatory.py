import typing

from bzd.validation.schema import Args, Constraint, ProcessedSchema
import bzd.validation.validation


class Mandatory(Constraint):

	def install(self, processedSchema: ProcessedSchema, args: Args) -> None:
		bzd.validation.validation.Validation(schema=[]).validate(args)
		processedSchema.setMandatory()
