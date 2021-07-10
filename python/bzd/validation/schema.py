import typing

Schema = typing.Dict[str, str]
ValidationCallable = typing.Callable[[str], bool]


class Constraint:

	def __init__(self, name: str, args: typing.List[str]) -> None:
		self.name = name
		self.args = args

	def install(self, processedSchema: "ProcessedSchema") -> typing.Optional[ValidationCallable]:
		return None


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
