import typing
from functools import partial

Result = typing.Optional[str]
Args = typing.List[str]
ValidationCallable = typing.Callable[[typing.Any, "Context"], Result]


class Context:

	def __init__(self, value: str) -> None:
		self.value = value
		self.underlying_: typing.Optional[typing.Any] = None

	@property
	def underlying(self) -> typing.Any:
		assert self.underlying_ is not None, "Underlying value is used but not set."
		return self.underlying_

	@property
	def resolvedValue(self) -> typing.Any:
		"""
		Return the value resolved, which is either the underlying value or the actual value,
		depending on whether the underlying exists.
		"""
		return self.value if self.underlying_ is None else self.underlying_


class TypeContext(Context):

	def setUnderlying(self, value: typing.Any) -> None:
		assert self.underlying_ is None
		self.underlying_ = value


class Constraint:

	def __init__(self, name: str, args: typing.List[str]) -> None:
		self.name = name
		self.args = args

	def install(self, processedSchema: "ProcessedSchema", args: Args) -> None:
		"""
		Install a contraint.
		"""
		assert False, "Constraint missing 'install' overload."

	@staticmethod
	def _toInteger(value: str) -> int:
		"""
		Convert to integer or throw.
		"""
		try:
			converted = float(value)
			assert converted.is_integer()
			return int(converted)
		except:
			raise Exception("The value '{}' is not a valid integer.".format(value))

	@staticmethod
	def _toFloat(value: str) -> float:
		"""
		Convert to integer or throw.
		"""
		try:
			return float(value)
		except:
			raise Exception("The value '{}' is not a valid floating point number.".format(value))


class ProcessedResult:

	def __init__(self) -> None:
		self.value: typing.Optional[typing.Any] = None
		self.errors: typing.List[str] = []

	def addError(self, error: str) -> None:
		self.errors.append(error)

	def setValue(self, value: typing.Any) -> None:
		assert self.value is None, "Value already set."
		self.value = value

	def __bool__(self) -> bool:
		return not bool(self.errors)


class ProcessedSchema:

	def __init__(self) -> None:
		self.mandatory_ = False
		self.type: typing.Optional[Constraint] = None
		self.validations: typing.List[typing.Callable[[Context], Result]] = []

	@property
	def isMandatory(self) -> bool:
		return self.mandatory_

	def setMandatory(self) -> None:
		"""
		Set this constraint as mandatory.
		"""
		assert self.mandatory_ == False, "Mandatory constraint already set."
		self.mandatory_ = True

	def setType(self, constraint: Constraint) -> None:
		"""
		Set a specific constraint as a type.
		"""

		assert self.type is None, "A type for this constraint is already specified."
		assert hasattr(constraint, "check"), "A type constraint must have a 'toType' method."
		self.type = constraint

	def install(self, constraints: typing.Dict[str, typing.Type[Constraint]], name: str, args: Args) -> None:
		"""
		Install a new constraint.
		"""

		# Check if the constraint is a type specialization.
		if self.type is not None:
			if hasattr(self.type, name):
				getattr(self.type, name)(self, args)
				return

		assert name in constraints, "Constraint of type '{}' is undefined.".format(name)

		constraint = constraints[name](name=name, args=args)
		constraint.install(self, args)

	def installValidation(self, validation: ValidationCallable, args: typing.Any) -> None:
		"""
		Add a validation callable to the list.
		"""

		bound = partial(validation, args)
		self.validations.append(bound)

	def validate(self, value: str) -> ProcessedResult:
		"""
		Validate a value.
		"""

		result = ProcessedResult()
		typeContext = TypeContext(value=value)

		# Process the type
		if self.type:
			resultType: typing.Optional[str] = self.type.check(context=typeContext)  # type: ignore
			if resultType is not None:
				result.addError(resultType)
				return result

		# Cast down the context to a normal context
		context = typing.cast(Context, typeContext)

		# Process the validation callables
		for validation in self.validations:
			maybeError = validation(context)
			if maybeError is not None:
				result.addError(maybeError)

		# If no error, set the value
		if result:
			result.setValue(context.resolvedValue)

		return result
