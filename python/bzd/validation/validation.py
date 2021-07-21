import typing
import re

from bzd.validation.schema import Constraint, ProcessedSchema, ProcessedResult
from bzd.validation.constraints.boolean import Boolean
from bzd.validation.constraints.integer import Integer
from bzd.validation.constraints.float import Float
from bzd.validation.constraints.string import String
from bzd.validation.constraints.mandatory import Mandatory

PATTERN_CONSTRAINT_ = re.compile(r"([a-zA-Z0-9_-]+)(?:\((.*)\))?")
SchemaDict = typing.Dict[str, str]
SchemaList = typing.List[str]
Schema = typing.Union[SchemaList, SchemaDict]

ValuesDict = typing.Dict[str, typing.Any]
ValuesList = typing.List[typing.Any]
Values = typing.Union[ValuesList, ValuesDict]


class ExceptionValidation(Exception):
	pass


class Result:

	def __init__(self, isList: bool) -> None:
		self.isList = isList
		self.globalErrors: typing.List[str] = []
		self.errors: typing.Dict[str, typing.List[str]] = {}
		self.values: typing.Dict[str, typing.Any] = {}

	def addGlobalError(self, error: str) -> None:
		self.globalErrors.append(error)

	def addError(self, key: str, errors: typing.List[str]) -> None:
		assert key not in self.errors, "Key '{}' already assigned.".format(key)
		self.errors[key] = errors

	def addResult(self, key: str, result: ProcessedResult) -> None:
		if result:
			assert key not in self.values, "Value already set."
			self.values[key] = result.value
		else:
			self.addError(key, result.errors)

	def __bool__(self) -> bool:
		return not bool(self.errors) and not bool(self.globalErrors)

	def __repr__(self) -> str:

		if self.isList:
			FORMAT_NOT_VALIDATE_SINGLE = "position {} does not validate: \"{}\""
			FORMAT_NOT_VALIDATE_MULTI = "Some positional values do not validate: {}"
		else:
			FORMAT_NOT_VALIDATE_SINGLE = "'{}' does not validate: \"{}\""
			FORMAT_NOT_VALIDATE_MULTI = "Some values do not validate: {}"

		content = []
		if len(self.globalErrors):
			content.extend(self.globalErrors)

		if len(self.errors.keys()) == 1:
			for key, result in self.errors.items():
				content.append(FORMAT_NOT_VALIDATE_SINGLE.format(key, ", ".join(result)))
		else:
			messages = ["{}: \"{}\"".format(key, ", ".join(result)) for key, result in self.errors.items()]
			content.append(FORMAT_NOT_VALIDATE_MULTI.format("; ".join(messages)))

		return "; ".join(content) if content else "no errors"


class Validation:

	AVAILABLE_CONSTRAINTS: typing.Dict[str, typing.Type[Constraint]] = {
		"boolean": Boolean,
		"integer": Integer,
		"float": Float,
		"string": String,
		"mandatory": Mandatory
	}

	def __init__(self, schema: Schema) -> None:
		self.processed: typing.Dict[str, ProcessedSchema] = {}
		self.isList = isinstance(schema, list)
		self._prepocessSchema(self._inputToInternal(schema))

	def mergeSchema(self, schema: Schema) -> None:
		"""
		Merge a schema with the existing validation.
		"""
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
		assert isinstance(constraints, str), "Constraint must be a string."
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

			if key not in self.processed:
				self.processed[key] = ProcessedSchema()

			# Parse each constraints strings
			parsed = Validation.parse(constraints)
			# Install the constraint
			for name, args in parsed.items():
				self.processed[key].install(constraints=self.AVAILABLE_CONSTRAINTS, name=name, args=args)

	def __len__(self) -> int:
		"""
		Get the number of entries.
		"""
		return len(self.processed.keys())

	def validate(self, values: Values, output: str = "throw") -> Result:
		"""
		Validates the values passed into argument.
		"""

		results = Result(self.isList)

		if self.isList and not isinstance(values, list):
			results.addGlobalError("Input must be a list, got '{}' instead.".format(type(values)))
		elif not self.isList and not isinstance(values, dict):
			results.addGlobalError("Input must be a dictionary, got '{}' instead.".format(type(values)))

		# Convert the inputs into internal type
		internals = self._inputToInternal(values)

		if results:
			for key, value in internals.items():
				if key in self.processed:
					result = self.processed[key].validate(value=value)
					results.addResult(key, result)
				else:
					results.addError(key, ["Value not expected."])

			# Check for mandatory values
			for key, constraints in self.processed.items():
				if constraints.isMandatory and key not in internals:
					results.addError(key, ["Missing mandatory value."])

		# If some errors are detected.
		if output == "throw":
			if not results:
				raise ExceptionValidation(str(results))
		elif output == "return":
			pass
		else:
			assert False, "Unsupported output type '{}'.".format(output)

		return results
