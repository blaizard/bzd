import typing


class BuiltinsBtl:
	"""Builtins utility functions to be used within the template, available under `btl` namespace."""

	@staticmethod
	def merge(*args: typing.Any) -> typing.Any:
		"""Group multiple container together."""

		assert (len(args) > 1), "The builtin function 'merge' requires at least 2 arguments."
		first = args[0]
		assert all(
		    isinstance(i, type(first))
		    for i in args[1:]), "The builtin function 'merge' requires all its elements to be of identical types."

		if isinstance(first, list):
			return sum(args, [])

		if isinstance(first, dict):
			return dict(map(dict.popitem, args))

		assert (False), f"The builtin function 'merge' cannot take arguments of type '{type(first)}'."

	@staticmethod
	def set(*args: typing.Any) -> typing.Set[typing.Any]:
		"""Create a set from multiple elements."""

		return set(args)

	@staticmethod
	def any(*args: typing.Any) -> bool:
		"""Checks if any of the argument evaluates to true."""

		return any(args)

	@staticmethod
	def all(*args: typing.Any) -> bool:
		"""Checks if all of the argument evaluates to true."""

		return all(args)

	@staticmethod
	def assertTrue(condition: typing.Any, message: str) -> None:
		"""Asserts that condition evaluates to true, otherwise terminates and print a message."""

		if not bool(condition):
			raise AssertionError(message)

	@staticmethod
	def error(message: str) -> None:
		"""Print an unconditional error message and terminates."""

		raise Exception(message)

	@staticmethod
	def unreachable() -> None:
		"""Print an unreachable error message and terminates."""

		BuiltinsBtl.error("This line should never be reached.")


Builtins = {"btl": BuiltinsBtl}
