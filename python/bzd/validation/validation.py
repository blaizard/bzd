import typing
import re

from bzd.validation.schema import Constraint, Schema, ProcessedSchema
from bzd.validation.constraints.integer import Integer

PATTERN_CONSTRAINT_ = re.compile(r"([a-zA-Z0-9_-]+)(?:\((.*)\))?")


class Validation:

	def __init__(self, schema: Schema) -> None:
		self.availableConstraints: typing.Dict[str, typing.Type[Constraint]] = {"integer": Integer}
		self.processed = self._prepocessSchema(schema)

	def _prepocessSchema(self, schema: Schema) -> typing.Dict[str, ProcessedSchema]:
		"""
		Preprocess the schema.
		"""

		processed: typing.Dict[str, ProcessedSchema] = {}

		for key, constraints in schema.items():

			processed[key] = ProcessedSchema()

			# Parse each constraints strings
			assert isinstance(constraints, str), "Constraint must be a string."
			constraintList = [constraint for constraint in constraints.split() if constraint]
			for constraint in constraintList:
				m = PATTERN_CONSTRAINT_.match(constraint)
				assert m, "Constraint format error: '{}'.".format(constraint)
				name = m.group(1)
				args = [arg for arg in (m.group(2) if m.group(2) else "").split(",") if arg]

				# Install the constraint
				processed[key].install(constraints=self.availableConstraints, name=name, args=args)

		return processed

	def validate(self, values: typing.Dict[str, typing.Any]) -> bool:

		results = []
		for key, value in values.items():
			result = self.processed[key].validate(value=value)
			if result is not None:
				results.append(result)

		return len(results) == 0
