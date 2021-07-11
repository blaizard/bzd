import typing

Schema = typing.Dict[str, str]
Result = typing.Optional[str]
ValidationCallable = typing.Callable[["Context"], Result]


class Context:

	def __init__(self, value: str) -> None:
		self.value = value
		self.underlying: typing.Optional[typing.Any] = None


class Constraint:

	def __init__(self, name: str, args: typing.List[str]) -> None:
		self.name = name
		self.args = args

	def install(self, processedSchema: "ProcessedSchema") -> None:
		"""
		Install a contraint.
		"""
		assert False, "Constraint missing 'install' overload."


class ProcessedSchema:

	def __init__(self) -> None:
		self.mandatory = False
		self.type: typing.Optional[Constraint] = None
		self.validations: typing.List[ValidationCallable] = []

	def setType(self, constraint: Constraint) -> None:
		"""
		Set a specific constraint as a type.
		"""

		assert self.type is None, "A type for this constraint is already specified."
		assert hasattr(constraint, "toType"), "A type constraint must have a 'toType' method."
		self.type = constraint

	def install(self, constraints: typing.Dict[str, typing.Type[Constraint]], name: str,
		args: typing.List[str]) -> None:
		"""
		Install a new constraint.
		"""

		# Check if the constraint is a type specialization.
		if self.type is not None:
			if hasattr(self.type, name):
				self.installValidation(getattr(self.type, name))
				return

		assert name in constraints, "Constraint of type '{}' is undefined.".format(name)

		constraint = constraints[name](name=name, args=args)
		constraint.install(self)

	def installValidation(self, validation: ValidationCallable) -> None:
		"""
		Add a validation callable to the list.
		"""

		self.validations.append(validation)

	def validate(self, value: str) -> Result:
		"""
		Validate a value.
		"""

		context = Context(value=value)

		# Process the
		self.underlying = None
		if self.type:
			context.underlying = self.type.toType(context=context)  # type: ignore
			if context.underlying is None:
				return "Not matching type."

		# Process the validation callables
		for validation in self.validations:
			result = validation(context)
			if result is not None:
				return result

		return None
