import typing
import re

from bzd.validation.schema import Constraint, Schema, ProcessedSchema
from bzd.validation.constraints.integer import Integer
from bzd.validation.constraints.float import Float
from bzd.validation.constraints.string import String

PATTERN_CONSTRAINT_ = re.compile(r"([a-zA-Z0-9_-]+)(?:\((.*)\))?")


class ExceptionValidation(Exception):
	pass


class Result:

	def __init__(self) -> None:
		self.errors: typing.Dict[str, typing.List[str]] = {}

	def add(self, key: str, errors: typing.List[str]) -> None:
		assert key not in self.errors, "Key '{}' already assigned.".format(key)
		self.errors[key] = errors

	def __bool__(self) -> bool:
		return not bool(self.errors)

	def __repr__(self) -> str:

		if len(self.errors.keys()) == 1:
			for key, result in self.errors.items():
				return "'{}' does not validate: \"{}\"".format(key, ", ".join(result))
		messages = ["{}: \"{}\"".format(key, ", ".join(result)) for key, result in self.errors.items()]
		return "Some values do not validate: {}".format("; ".join(messages))


class Validation:

	def __init__(self, schema: Schema) -> None:
		self.availableConstraints: typing.Dict[str, typing.Type[Constraint]] = {
			"integer": Integer,
			"float": Float,
			"string": String
		}
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

	def validate(self, values: typing.Dict[str, typing.Any], output: str = "throw") -> Result:
		"""
		Validates the values passed into argument.
		"""

		results = Result()
		for key, value in values.items():
			if key in self.processed:
				result = self.processed[key].validate(value=value)
				if result is not None:
					results.add(key, result)
			else:
				results.add(key, ["value not expected"])

		# If some errors are detected.
		if output == "throw":
			if not results:
				raise ExceptionValidation(str(results))
		elif output == "return":
			pass
		else:
			assert False, "Unsupported output type '{}'.".format(output)

		return results
