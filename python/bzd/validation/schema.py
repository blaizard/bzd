import typing
from functools import partial

Schema = typing.Dict[str, str]
Result = typing.Optional[str]
Args = typing.List[str]
ValidationCallable = typing.Callable[[typing.Any, "Context"], Result]


class Context:

	def __init__(self, value: str) -> None:
		self.value = value
		self.underlying_: typing.Optional[typing.Any] = None

	@property
	def underlying(self) -> typing.Any:
		assert self.underlying_
		return self.underlying_

class Constraint:

	def __init__(self, name: str, args: typing.List[str]) -> None:
		self.name = name
		self.args = args

	def install(self, processedSchema: "ProcessedSchema", args: Args) -> None:
		"""
		Install a contraint.
		"""
		assert False, "Constraint missing 'install' overload."


class ProcessedSchema:

	def __init__(self) -> None:
		self.mandatory = False
		self.type: typing.Optional[Constraint] = None
		self.validations: typing.List[typing.Callable[[Context], Result]] = []

	def setType(self, constraint: Constraint) -> None:
		"""
		Set a specific constraint as a type.
		"""

		assert self.type is None, "A type for this constraint is already specified."
		assert hasattr(constraint, "toType"), "A type constraint must have a 'toType' method."
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

	def validate(self, value: str) -> Result:
		"""
		Validate a value.
		"""

		context = Context(value=value)

		# Process the
		if self.type:
			context.underlying_ = self.type.toType(context=context)  # type: ignore
			if context.underlying_ is None:
				return "Not matching type."

		# Process the validation callables
		for validation in self.validations:
			result = validation(context)
			if result is not None:
				return result

		return None
