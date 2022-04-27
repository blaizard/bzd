import typing
import re

from bzd.validation.result import Result
from bzd.validation.schema import Constraint, ProcessedSchema, ProcessedResult, Values, ValuesList, ValuesDict
from bzd.validation.constraints.boolean import Boolean
from bzd.validation.constraints.integer import Integer
from bzd.validation.constraints.float import Float
from bzd.validation.constraints.string import String
from bzd.validation.constraints.mandatory import Mandatory

PATTERN_CONSTRAINT_ = re.compile(r"([a-zA-Z0-9_-]+)(?:\((.*)\))?")


class ExceptionValidation(Exception):
	pass


SchemaDict = typing.Dict[str, str]
SchemaList = typing.List[str]
Schema = typing.TypeVar("Schema", SchemaList, SchemaDict)


class Validation(typing.Generic[Schema]):

	AVAILABLE_CONSTRAINTS: typing.Dict[str, typing.Type[Constraint]] = {
		"boolean": Boolean,
		"integer": Integer,
		"float": Float,
		"string": String,
		"mandatory": Mandatory
	}

	memoizedProcessedSchema_: typing.Dict[typing.Tuple[str, int], ProcessedSchema] = {}

	def __init__(self, schema: Schema, args: typing.Any = None) -> None:
		"""Initialize the Validation object.

		Args:
			schema: The schema to be used.
			args: Arguments to be passed to the constraints.
		"""

		self.processed: typing.Dict[str, ProcessedSchema] = {}
		self.isList = isinstance(schema, list)
		# Arguments to be passed the the check and process functions.
		# It should not be passed into the install function, otherwise this would
		# make memoization invalid.
		self.args = args
		self._prepocessSchema(self._inputToInternal(schema))

	def _inputToInternal(self, schema: Values) -> ValuesDict:
		"""
		Convert a user provided schema into an internal schema.
		"""
		if isinstance(schema, list):
			return {str(i): value for i, value in enumerate(schema)}
		return schema

	@staticmethod
	def parse(constraints: str) -> typing.Dict[str, typing.List[str]]:
		"""
		Parse string formatted contract and return the parsed data structure.
		Note, it is ok to use a dictionary as dicts preserve insertion order in Python 3.7+
		"""
		assert isinstance(constraints, str), f"Constraint must be a string, received: {str(constraints)}"
		constraintList = [constraint for constraint in constraints.split() if constraint]
		result = {}
		for constraint in constraintList:
			m = PATTERN_CONSTRAINT_.match(constraint)
			assert m, "Constraint format error: '{}'.".format(constraint)
			name = m.group(1)
			args = [arg for arg in (m.group(2) if m.group(2) else "").split(",") if arg]
			assert name not in result, "Constraint '{}' has been set twice.".format(name)
			result[name] = args

		return result

	def _prepocessSchema(self, schema: SchemaDict) -> None:
		"""
		Preprocess the schema.
		"""

		for key, constraints in schema.items():

			# The key must include all variability to generate the processed schema.
			memoizedKey = (constraints, id(self.AVAILABLE_CONSTRAINTS))

			if memoizedKey in Validation.memoizedProcessedSchema_ and False:
				self.processed[key] = Validation.memoizedProcessedSchema_[memoizedKey]

			else:
				self.processed[key] = ProcessedSchema()

				# Parse each constraints strings
				parsed = Validation.parse(constraints)
				# Install the constraint
				for name, args in parsed.items():
					self.processed[key].install(constraints=self.AVAILABLE_CONSTRAINTS, name=name, args=args)

				# Save the processed schema
				Validation.memoizedProcessedSchema_[memoizedKey] = self.processed[key]

	def __len__(self) -> int:
		"""
		Get the number of entries.
		"""
		return len(self.processed.keys())

	def validate(self, values: Values, output: str = "throw") -> Result[Schema]:
		"""
		Validates the values passed into argument.
		"""
		results = Result[Schema](self.isList)

		if self.isList and not isinstance(values, list):
			results.addGlobalError("Input must be a list, got '{}' instead.".format(type(values)))
		elif not self.isList and not isinstance(values, dict):
			results.addGlobalError("Input must be a dictionary, got '{}' instead.".format(type(values)))

		# Convert the inputs into internal type
		internals = self._inputToInternal(values)

		if results:
			for key, value in internals.items():
				if key in self.processed:
					result = self.processed[key].validate(value=value, args=self.args)
					results.addResult(key, result)
				elif "*" in self.processed:
					result = self.processed["*"].validate(value=value, args=self.args)
					results.addResult(key, result)
				elif not self.processed:
					results.addError(key, ["no value expected."])
				else:
					results.addError(key, [
						"value not expected, valid choices are: {}.".format(", ".join(
						["'{}'".format(x) for x in self.processed.keys()]))
					])

			# Check for mandatory values
			for key, constraints in self.processed.items():
				if constraints.isMandatory and key not in internals:
					results.addError(key, ["missing mandatory value."])

		# If some errors are detected.
		if output == "throw":
			if not results:
				raise ExceptionValidation(str(results))
		elif output == "return":
			pass
		else:
			assert False, "Unsupported output type '{}'.".format(output)

		return results
