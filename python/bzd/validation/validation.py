import typing
import re

Schema = typing.Dict[str, str]
ValidationCallable = typing.Callable[[str], bool]

PATTERN_CONSTRAINT_ = re.compile(r"([a-zA-Z0-9_-]+)(?:\((.*)\))?")


class Constraint:

	def __init__(self, name: str, args: typing.List[str]) -> None:
		self.name = name
		self.args = args

	def install(self, processedSchema: "ProcessedSchema") -> typing.Optional[ValidationCallable]:
		return None


class Integer(Constraint):

	def install(self, processedSchema: "ProcessedSchema") -> typing.Optional[ValidationCallable]:
		return lambda value: True


class ProcessedSchema:

	def __init__(self) -> None:
		self.mandatory = False
		self.type = "string"
		self.constraints: typing.List[Constraint] = []
		self.validations: typing.List[ValidationCallable] = []

	def append(self, constraint: Constraint) -> None:
		self.constraints.append(constraint)

	def install(self) -> None:
		validations: typing.List[ValidationCallable] = []
		for constraint in self.constraints:
			maybeValidation = constraint.install(self)
			if maybeValidation:
				self.validations.append(maybeValidation)


class Validation:

	def __init__(self, schema: Schema) -> None:
		self.availableConstraints = {"integer": Integer}
		self.processed = self._prepocessSchema(schema)

	def _prepocessSchema(self, schema: Schema) -> typing.Dict[str, ProcessedSchema]:
		"""
		Preprocess the schema.
		"""

		processed = {}

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

				# Register the constraint
				assert name in self.availableConstraints, "Constraint of type '{}' is undefined.".format(name)
				processed[key].append(self.availableConstraints[name](name=name, args=args))

		# Install the various constaints
		processed[key].install()

		return processed

	def validate(self, values: typing.Dict[str, typing.Any]) -> bool:

		for key, value in values.items():
			pass

		return True
