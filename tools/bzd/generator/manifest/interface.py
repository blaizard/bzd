from typing import Optional, Sequence, Set, TYPE_CHECKING

if TYPE_CHECKING:
	from tools.bzd.generator.manifest.manifest import Manifest

"""
Represents an interface
"""


class Interface(object):

	def __init__(self, manifest: "Manifest", identifier: str) -> None:
		self.manifest: Optional["Manifest"] = manifest
		self.identifier = identifier
		self.definition = manifest.getData().get("interfaces", {}).get(identifier, {})

	def isValid(self) -> bool:
		return self.identifier != None

	def getName(self) -> str:
		return self.identifier

	def getIncludes(self) -> Sequence[str]:
		includes = self.definition.get("includes", [])
		return includes if isinstance(includes, list) else [includes]

	def _getDerivedImplementation(self, name: str, interfaces: Set[str]) -> Optional[str]:
		assert self.manifest
		if self.manifest.isInterface(name):
			interface = self.manifest.getInterface(name)
			derivedImplementation = interface.getImplementation()
			if derivedImplementation:
				assert derivedImplementation not in interfaces, "There is a loop in the implementation inheritance for this interface: {}".format(
					", ".join(list(interfaces)))
				interfaces.add(derivedImplementation)
				return self._getDerivedImplementation(derivedImplementation, interfaces)
		return name

	def getImplementation(self) -> Optional[str]:
		implementation = self.definition.get("implementation", None)
		return self._getDerivedImplementation(implementation, set()) if implementation else None

	def getImplementationOrInterface(self) -> str:
		implementation = self.getImplementation()
		return implementation if implementation else self.getName()


"""
Helper to ceate an empty interface
"""


class EmptyInterface(Interface):

	def __init__(self, identifier: str) -> None:
		self.manifest: Optional["Manifest"] = None
		self.identifier = identifier
		self.definition = {}
