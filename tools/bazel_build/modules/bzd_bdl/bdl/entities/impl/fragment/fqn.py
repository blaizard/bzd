import random
import time

_staticUid = 0


class FQN:
	@staticmethod
	def makeUniquePrivate() -> str:
		"""
		Generate a unique FQN for this translation unit.
		"""
		# The '_' is to tell that the visibility of this object is limited to this file,
		# while the '~' is to ensure no name collision with what the user can define.
		global _staticUid
		fqn = f"_{_staticUid:d}~"
		_staticUid += 1
		return fqn

	@staticmethod
	def makeUnique(namespace: list[str]) -> str:
		"""
		Generate a unique FQN for composition for this translation unit.
		"""
		global _staticUid
		name = f"{time.time_ns():d}-{random.randint(0, 1000000):d}-{_staticUid:d}~"
		_staticUid += 1
		return FQN.fromNamespace(namespace=namespace, name=name)

	@staticmethod
	def isPrivate(fqn: str) -> bool:
		return fqn.startswith("_")

	@staticmethod
	def isLocal(fqn: str) -> bool:
		return fqn.endswith("~")

	@staticmethod
	def fromNamespace(namespace: list[str], name: str | None = None) -> str:
		"""
		Make the fully qualified name from a symbol name
		"""
		if name is None:
			return ".".join(namespace)
		return ".".join(namespace + [name])

	@staticmethod
	def toNamespace(fqn: str) -> list[str]:
		"""
		Convert a FQN string into a namespace.
		"""
		return fqn.split(".")
