#!/usr/bin/python

from .validator import ValidatorReference, ValidatorCustom, ValidatorObject, ValidatorInterface

from typing import Sequence, Callable, Any, Optional, Mapping, Set, Dict, List, Iterable, TYPE_CHECKING
if TYPE_CHECKING:
	from .manifest import Manifest
	from .interface import Interface
"""
Represents an object
"""


class Object():

	def __init__(self, manifest: "Manifest", identifier: str, definition: Optional[Mapping[str, Any]] = None) -> None:
		self.manifest = manifest
		self.identifier = identifier
		self.definition = definition if definition else manifest.getData().get("objects", {}).get(identifier, {})
		self.deps = {"interface": set([self.getInterfaceName()]), "object": {}}

		# Process the definitions
		def visit(value: Any) -> None:
			if ValidatorReference.isMatch(value):
				valueStr = value.getRepr()

				# If this represents an object
				if ValidatorObject.isMatch(valueStr):
					result = ValidatorObject.parse(valueStr)
					assert result
					assert isinstance(self.deps["interface"], set)
					self.deps["interface"].add(result["interface"])
					assert isinstance(self.deps["object"], dict)
					self.deps["object"][result["interface"]] = self.deps["object"].get(result["interface"], set())
					self.deps["object"][result["interface"]].add(result["name"])
					value.setReprCallback(
						self.manifest, lambda renderer: renderer.get("object", "").format(interface=result["interface"],
						name=result["name"]))

				# If this represents an interface
				elif ValidatorInterface.isMatch(valueStr):
					assert isinstance(self.deps["interface"], set)
					self.deps["interface"].add(valueStr)

			elif ValidatorCustom.isMatch(value):
				value.setReprCallback(self.manifest, lambda renderer: renderer.get(value.type))

		self._walk(self.definition, visit)

		# Sanity check
		assert ((self.definition.get("config", None) != None) and (self.definition.get("params", None) != None)
				) == False, "Object cannot have a configuration (config) and parameters (params) at the same time."

	"""
	Return the interface of the object
	"""

	def getInterface(self) -> "Interface":
		name = self.getInterfaceName()
		return self.manifest.getInterface(name, mustExists=False)

	"""
	Return the interface of the object
	"""

	def getInterfaceName(self) -> str:
		return self.identifier.split(".")[0]

	"""
	Walk through all values of a dictionary and call a callback
	"""

	@staticmethod
	def _walk(obj: Iterable[Any], callback: Callable[[Any], None]) -> None:
		for value in (obj.values() if isinstance(obj, dict) else obj):
			if isinstance(value, dict):
				Object._walk(value, callback)
			elif isinstance(value, list):
				Object._walk(value, callback)
			else:
				callback(value)

	"""
	Get all dependent interfaces
	"""

	def getDependentInterfaces(self) -> List["Interface"]:
		return [
			self.manifest.getInterface(dependency) for dependency in self.deps["interface"]
			if self.manifest.isInterface(dependency)
		]

	"""
	Get all dependent objects
	"""

	def getDependentObjects(self) -> Dict[str, Set[str]]:
		return self.deps["object"]  # type: ignore

	"""
	Return the name of the object
	"""

	def getName(self) -> str:
		return self.identifier.split(".")[1]

	"""
	Return the object config if any, or None otherwise.
	"""

	def getConfig(self) -> Optional[Dict[str, Any]]:
		return self.definition.get("config", None)  # type: ignore

	"""
	Get the implementation class for this object. Go as deep as the inheritance goes.
	"""

	def getImplementation(self) -> str:
		implementationInterface = self.getInterface().getImplementation()
		implementationObjecy = self.definition.get("implementation", None)
		assert implementationInterface == None or implementationObjecy == None, "Both interface implementation '{}' and object implementation '{}' cannot be set at the same time for object identifier '{}'.".format(
			implementationInterface, implementationObjecy, self.identifier)
		implementation = implementationInterface if implementationInterface else self.definition.get(
			"implementation", self.getInterfaceName())
		return self.manifest.getInterface(  # type: ignore
			implementation, mustExists=False).getImplementationOrInterface()

	"""
	Get parameters
	"""

	def getParams(self) -> List[str]:
		return self.definition.get("params", [])  # type: ignore
