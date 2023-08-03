import typing
import time
import random

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
		fqn = "_{:d}~".format(_staticUid)
		_staticUid += 1
		return fqn

	@staticmethod
	def makeUnique(namespace: typing.List[str]) -> str:
		"""
        Generate a unique FQN for composition for this translation unit.
        """
		global _staticUid
		name = "{:d}-{:d}-{:d}~".format(time.time_ns(), random.randint(0, 1000000), _staticUid)
		_staticUid += 1
		return FQN.fromNamespace(namespace=namespace, name=name)

	@staticmethod
	def isPrivate(fqn: str) -> bool:
		return fqn.startswith("_")

	@staticmethod
	def isLocal(fqn: str) -> bool:
		return fqn.endswith("~")

	@staticmethod
	def fromNamespace(namespace: typing.List[str], name: typing.Optional[str] = None) -> str:
		"""
        Make the fully qualified name from a symbol name
        """
		if name is None:
			return ".".join(namespace)
		return ".".join(namespace + [name])

	@staticmethod
	def toNamespace(fqn: str) -> typing.List[str]:
		"""
        Convert a FQN string into a namespace.
        """
		return fqn.split(".")
