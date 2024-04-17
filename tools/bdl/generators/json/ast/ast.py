import typing

Json = typing.Dict[str, typing.Any]


class Parameters:

	def __init__(self, data: typing.List[Json]) -> None:
		self.data = data

	def get(self, name: str, default: typing.Any = None) -> typing.Any:
		for param in self.data:
			if param.get("name") == name:
				return param.get("value", default)
		return default

	def __getattr__(self, name: str) -> typing.Any:
		for param in self.data:
			if param.get("name") == name and "value" in param:
				return param["value"]
		raise KeyError(f"Unknown attribute '{name}'.")


class Expression:

	def __init__(self, data: Json) -> None:
		self.data = data

	@property
	def symbol(self) -> str:
		return self.data["symbol"]

	@property
	def parameters(self) -> Parameters:
		return Parameters(self.data.get("parameters", []))


class ExpressionEntry:

	def __init__(self, data: Json) -> None:
		self.data = data

	@property
	def expression(self) -> Expression:
		return Expression(self.data["expression"])


class Context:

	def __init__(self, data: Json) -> None:
		self.data = data

	@property
	def workloads(self) -> typing.Iterator[ExpressionEntry]:
		for workload in self.data.get("workloads", []):
			yield ExpressionEntry(workload)

	@property
	def services(self) -> typing.Iterator[ExpressionEntry]:
		for service in self.data.get("services", []):
			yield ExpressionEntry(service)


class Ast:

	def __init__(self, data: Json) -> None:
		self.data = data

	@property
	def contexts(self) -> typing.Iterator[Context]:
		for context in self.data.get("contexts", []):
			yield Context(context)
