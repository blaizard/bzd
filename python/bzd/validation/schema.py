import typing
from functools import partial

from bzd.utils.decorators import cached_classproperty

if typing.TYPE_CHECKING:
	from bzd.validation.result import Result

ValuesDict = typing.Dict[str, typing.Any]
ValuesList = typing.List[typing.Any]
Values = typing.Union[ValuesList, ValuesDict]

ValidationCallable = typing.Callable[[typing.Any, "Context"], typing.Optional[str]]


class Context:

	def __init__(self, value: str, args: typing.Any) -> None:
		self.value = value
		self.args = args
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


T = typing.TypeVar("T", typing.List[typing.Any], typing.Dict[typing.Any, typing.Any])


class Constraint:
	constraints: typing.Dict[str, typing.Type["Constraint"]] = {}

	def __init__(self, name: str) -> None:
		self.name = name

	def install(self, processedSchema: "ProcessedSchema", args: typing.List[str]) -> None:
		"""
        Install a constraint.
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
			raise Exception("expects an integer, received: '{}'.".format(value))

	@staticmethod
	def _toFloat(value: str) -> float:
		"""
        Convert to integer or throw.
        """
		try:
			return float(value)
		except:
			raise Exception("expects a floating point number, received: '{}'.".format(value))

	@staticmethod
	def validate(schema: T, values: Values) -> "Result[T]":
		import bzd.validation.validation

		return bzd.validation.validation.Validation(schema=schema).validate(values)


class NoopConstraint(Constraint):

	def install(self, processedSchema: "ProcessedSchema", args: typing.List[str]) -> None:
		pass


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
		self.validations: typing.List[typing.Callable[[Context], typing.Optional[str]]] = []

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

		assert (self.type is None), f"A type for this constraint is already specified: {str(self.type)}"
		self.type = constraint

	def install(
	    self,
	    constraints: typing.Dict[str, typing.Type[Constraint]],
	    name: str,
	    args: typing.List[str],
	) -> None:
		"""
        Install a new constraint.
        """

		# Check if the constraint is a type specialization, this gets the priority over
		# globally available constraints.
		if self.type is not None and name in self.type.constraints:
			constraintClass = self.type.constraints[name]
		else:
			assert name in constraints, "Constraint of type '{}' is undefined.".format(name)
			constraintClass = constraints[name]

		constraint = constraintClass(name=name)

		# Install
		constraint.install(self, args)

		# Add any 'check' function to the validation.
		if hasattr(constraint, "check"):
			self.validations.append(constraint.check)

	def installValidation(self, validation: ValidationCallable, args: typing.Any) -> None:
		"""
        Add a validation callable to the list.
        """

		bound = partial(validation, args)
		self.validations.append(bound)

	def validate(self, value: str, args: typing.Any) -> ProcessedResult:
		"""
        Validate a value.
        """

		result = ProcessedResult()
		context = TypeContext(value=value, args=args)

		# Process the validation callables
		for validation in self.validations:
			try:
				validation(context=context)  # type: ignore
			except BaseException as e:
				result.addError(str(e))

		# If no error, set the value
		if result:
			result.setValue(context.resolvedValue)

		return result
