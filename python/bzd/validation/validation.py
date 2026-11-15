import re
import typing

from bzd.validation.constraints.boolean import Boolean
from bzd.validation.constraints.float import Float
from bzd.validation.constraints.integer import Integer
from bzd.validation.constraints.mandatory import Mandatory
from bzd.validation.constraints.string import String
from bzd.validation.result import Result
from bzd.validation.schema import (
	Constraint,
	ProcessedSchema,
	Values,
	ValuesDict,
)

PATTERN_CONSTRAINT_ = re.compile(r"([a-zA-Z0-9_-]+)(?:\((.*)\))?")


class ExceptionValidation(Exception):
	pass


SchemaDict = dict[str, str]
SchemaList = list[str]
Schema = typing.TypeVar("Schema", SchemaList, SchemaDict)


class Validation(typing.Generic[Schema]):
	AVAILABLE_CONSTRAINTS: typing.ClassVar[dict[str, type[Constraint]]] = {
		"boolean": Boolean,
		"integer": Integer,
		"float": Float,
		"string": String,
		"mandatory": Mandatory,
	}

	memoizedProcessedSchema_: typing.ClassVar[dict[tuple[str, int], ProcessedSchema]] = {}

	def __init__(self, schema: Schema, args: typing.Any = None) -> None:
		"""Initialize the Validation object.

		Args:
		        schema: The schema to be used.
		        args: Arguments to be passed to the constraints.
		"""

		self.processed: dict[str, ProcessedSchema] = {}
		self.isList = isinstance(schema, list)
		# Arguments to be passed to the check and process functions.
		# It should not be passed into the install function, otherwise this would
		# make memoization invalid.
		self.args = args
		# This is only used for debug purposes.
		self.schema: dict[str, str] = self._inputToInternal(schema)
		self._prepocessSchema(self.schema)

	def _inputToInternal(self, schema: Values) -> ValuesDict:
		"""
		Convert a user provided schema into an internal schema.
		"""
		if isinstance(schema, list):
			return {str(i): value for i, value in enumerate(schema)}
		return schema

	@staticmethod
	def parse(constraints: str) -> dict[str, list[str]]:
		"""
		Parse string formatted contract and return the parsed data structure.
		Note, it is ok to use a dictionary as dicts preserve insertion order in Python 3.7+
		"""
		assert isinstance(constraints, str), f"Constraint must be a string, received: {constraints!s}"
		constraintList = [constraint for constraint in constraints.split() if constraint]
		result = {}
		for constraint in constraintList:
			m = PATTERN_CONSTRAINT_.match(constraint)
			assert m, f"Constraint format error: '{constraint}'."
			name = m.group(1)
			args = [arg for arg in (m.group(2) if m.group(2) else "").split(",") if arg]
			assert name not in result, f"Constraint '{name}' has been set twice."
			result[name] = args

		return result

	def _prepocessSchema(self, schema: SchemaDict) -> None:
		"""
		Preprocess the schema.
		"""

		for key, constraints in schema.items():
			# The key must include all variability to generate the processed schema.
			memoizedKey = (constraints, id(self.AVAILABLE_CONSTRAINTS))

			if False:
				self.processed[key] = Validation.memoizedProcessedSchema_[memoizedKey]

			else:
				self.processed[key] = ProcessedSchema()

				# Parse each constraint string.
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
			results.addGlobalError(f"Input must be a list, got '{type(values)}' instead.")
		elif not self.isList and not isinstance(values, dict):
			results.addGlobalError(f"Input must be a dictionary, got '{type(values)}' instead.")

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
					results.addError(
						key,
						["value not expected, valid choices are: {}.".format(", ".join([f"'{x}'" for x in self.processed]))],
					)

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
			assert False, f"Unsupported output type '{output}'."

		return results

	def __str__(self) -> str:
		attrs = " ".join([f'{k}="{v}"' for k, v in self.schema.items()])
		return f"<Validation {attrs} />"
